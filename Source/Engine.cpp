// File Name:    Engine.cpp
// Author(s):    main Taro Omiya, secondary Niko Bekris, Steven Yacoub, Boyuan Hu
// Course:       GAM541
// Project:      RASS
// Purpose:      Core engine class coordinating all game systems.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "Engine.h"

#include <cstdio>
#include <memory>
#include <nlohmann/json.hpp>
#include <print>
#include <utility>
#include <GLFW/glfw3.h>

#include "Events/FixedUpdate.h"
#include "Events/Global.h"
#include "Events/GlobalEventArgs.h"
#include "Events/GlobalEventID.h"
#include "Events/Render.h"
#include "Events/Update.h"
#include "Systems/GlobalEvents/IGlobalEventsSystem.h"
#include "Systems/Logging/ILoggingSystem.h"
#include "Systems/Render/IRenderSystem.h"
#include "Systems/Time/ITimeSystem.h"
#include "Systems/Manager/SystemsManager.h"
#include "Systems/Scene/ISceneSystem.h"
#include "Utils.h"

using json = nlohmann::json;

namespace RassEngine {

using namespace Systems;

Engine::Engine(std::unique_ptr<Systems::SystemsManager> &&systems) {
	// Set systems if not null
	if(systems != nullptr) {
		systemsManager = std::move(systems);
		return;
	}

	// Otherwise, log an error
	if(ILoggingSystem::Get() != nullptr) {
		LOG_ERROR("Called {}::setSystemsManager() with nullptr.", NAMEOF(Engine));
		return;
	}

	// The worst-case scenario, print the error to console directly
	std::println(stderr, "Error: called {}::setSystemsManager() with nullptr.", NAMEOF(Engine));
}

Engine::~Engine() {
	if(systemsManager != nullptr) {
		systemsManager.reset(nullptr);
	}
}

bool Engine::canRun() const {
	// Make sure the systems manager is setup correctly
	if(systemsManager == nullptr) {
		std::println(stderr, "Error: {} doesn't have a {} instance.", NAMEOF(Engine), NAMEOF(Systems::SystemsManager));
		return false;
	}

	// Make sure the logging system is defined
	if(ILoggingSystem::Get() == nullptr) {
		std::println(stderr, "Error: {} doesn't have a {} registered.", NAMEOF(Systems::SystemsManager), NAMEOF(Systems::ILoggingSystem));
		return false;
	}

	// At this point, we can now assume the logging system is available
	// Make sure the events are defined
	if(IGlobalEventsSystem::Get() == nullptr) {
		LOG_ERROR("{} doesn't have a {} registered.", NAMEOF(Systems::SystemsManager), NAMEOF(Systems::IGlobalEventsSystem));
		return false;
	}

	// Make sure the renderer is defined
	if(IRenderSystem::Get() == nullptr) {
		LOG_ERROR("{} doesn't have a {} registered.", NAMEOF(Systems::SystemsManager), NAMEOF(Systems::IRenderSystem));
		return false;
	}

	// Make sure the timesystem is defined
	if(ITimeSystem::Get() == nullptr) {
		LOG_ERROR("{} doesn't have a {} registered.", NAMEOF(Systems::SystemsManager), NAMEOF(Systems::ITimeSystem));
		return false;
	}

	// Make sure the scene is defined
	if(ISceneSystem::Get() == nullptr) {
		LOG_ERROR("{} doesn't have a {} registered.", NAMEOF(Systems::SystemsManager), NAMEOF(Systems::ISceneSystem));
		return false;
	}

	// Check if the scene system can run
	return ISceneSystem::Get()->IsRunning();
}

void Engine::run() {
	// Make sure everything is setup properly
	if(!canRun()) {
		// If not, indicate the reason
		LOG_ERROR("Engine is not setup correctly");

		// Call the global quit event
		IGlobalEventsSystem::Get()->call(Events::GlobalEventArgs(Events::Global::Quit));
		return;
	}

	// Might need to change this since chrono is technically not a system level library
	ITimeSystem::Get()->StartTrackingTime();

	// Run the game loop
	while(ISceneSystem::Get()->IsRunning()) {
		// Check whether we should run the fixed update events
		while(ITimeSystem::Get()->UpdateDeltaTime()) {
			// Call the fixed update events
			if(!CallEvent(Events::FixedUpdate::Before)) {
				break;
			} else if(!CallEvent(Events::FixedUpdate::On)) {
				break;
			} else if(!CallEvent(Events::FixedUpdate::After)) {
				break;
			}
		}

		// Call the update events
		if(!CallEvent(Events::Update::Before)) {
			break;
		} else if(!CallEvent(Events::Update::On)) {
			break;
		} else if(!CallEvent(Events::Update::After)) {
			break;
		}

		// Call the render events
		if(!CallEvent(Events::Render::Before)) {
			break;
		} else if(!IRenderSystem::Get()->BeginRender()) {
			LOG_ERROR("Engine halted after running IRenderSystem::BeginRender()");
			break;
		} else if(!CallEvent(Events::Render::On)) {
			break;
		} else if(!IRenderSystem::Get()->DrawRenderables()) {
			LOG_ERROR("Engine halted after running IRenderSystem::DrawRenderables()");
			break;
		} else if(!IRenderSystem::Get()->EndRender()) {
			LOG_ERROR("Engine halted after running IRenderSystem::EndRender()");
			break;
		} else if(!CallEvent(Events::Render::After)) {
			break;
		}

		// Wait for a bit to hit the target framerate, and not waste resources.
		ITimeSystem::Get()->EndFrame();
	}

	// Call the global quit event
	IGlobalEventsSystem::Get()->call(Events::GlobalEventArgs(Events::Global::Quit));
}

std::shared_ptr<GLFWwindow> Engine::CreateGlfwWindow() {
	// By default, return null
	std::shared_ptr<GLFWwindow> toReturn{nullptr};
	if(!glfwInit()) {
		LOG_ERROR("Failed to initialize GLFW");
		return toReturn;
	}

	// Configure GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Grab the primary monitor
	GLFWmonitor *monitor = glfwGetPrimaryMonitor();
	if(!monitor) {
		LOG_ERROR("Primary monitor was not detected");
		glfwTerminate();
		return toReturn;
	}

	// Retrieve the data on the primary monitor
	const GLFWvidmode *mode = glfwGetVideoMode(monitor);
	if(!mode) {
		LOG_ERROR("Unable to determine the primary monitor's screen size");
		glfwTerminate();
		return toReturn;
	}

	// Scoping so *window pointer cannot be accidentally used
	{
		// TODO: perhaps the arguments here should be read from a JSON file for window settings
		GLFWwindow *window =
#ifdef _DEBUG
			// Make a small game window in windowed mode
			glfwCreateWindow(1280, 720, "RASS", nullptr, nullptr);
#else
			// Make a game window in full-screen mode
			glfwCreateWindow(mode->width, mode->height, "RASS", monitor, nullptr);
#endif // _DEBUG

		if(!window) {
			LOG_ERROR("Failed to create GLFW window");
			glfwTerminate();
			return toReturn;
		}

		// Construct a shared pointer version of the window
		toReturn = std::shared_ptr<GLFWwindow>{window
			// Define a custom deleter of this window on clean-up
		, [] (GLFWwindow *ptr) {
			glfwDestroyWindow(ptr);
			glfwTerminate();
		}};
	}

	// Activate the OpenGL context for the window
	glfwMakeContextCurrent(toReturn.get());

	// Raw mouse motion (optional, reduces OS acceleration/scaling)
	if(glfwRawMouseMotionSupported()) {
		glfwSetInputMode(toReturn.get(), GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	}

	return toReturn;
}

bool Engine::CallEvent(const Events::GlobalEventID &id) const {
	bool toReturn = IGlobalEventsSystem::Get()->call(id);
	if(!toReturn) {
		LOG_ERROR("Engine halted after event, \"{}\"", id.GetName());
	}
	return toReturn;
}

}
