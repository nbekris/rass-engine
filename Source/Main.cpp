// File Name:    Main.cpp
// Author(s):    main Taro Omiya, secondary Niko Bekris, Boyuan Hu, Steven Yacoub, Eric Fleegal
// Course:       GAM541
// Project:      RASS
// Purpose:      Entry point for the RASS game application.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"

#include <glbinding/gl/functions.h>
#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>

// GLFW macros
#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <windows.h>
#include <print>

#include "Engine.h"
#include "Systems/Logging/ILoggingSystem.h"
#include "Systems/Logging/LoggingSystem.h"
#include "Systems/Entity/IEntityFactory.h"
#include "Systems/Entity/EntityFactory.h"
#include "Systems/Memory/MemorySystem.h"
#include "Systems/GlobalEvents/IGlobalEventsSystem.h"
#include "Systems/GlobalEvents/GlobalEventsSystem.h"
#include "Systems/Scene/ISceneSystem.h"
#include "Systems/Scene/SceneSystem.h"
#include "Systems/Input/IInputSystem.h"
#include "Systems/Input/InputSystemGlfw.h"
#include "Systems/Render/IRenderSystem.h"
#include "Systems/Render/RenderSystemOpenGl.h"
#include "Systems/Resource/IResourceSystem.h"
#include "Systems/Resource/ResourceSystem.h"
#include "Systems/DebugDraw/IDebugDrawSystem.h"
#include "Systems/DebugDraw/DebugDrawSystem.h"
#include "Systems/ImGui/IImGuiSystem.h"
#include "Systems/ImGui/ImGuiSystem.h"
#include "Systems/Manager/SystemsManagerBuilder.h"
#include "Systems/Time/ITimeSystem.h"
#include "Systems/Time/TimeSystem.h"
#include "Systems/Physics/IPhysicsSystem.h"
#include "Systems/Physics/PhysicsSystem.h"
#include "Systems/Audio/IAudioSystem.h"
#include "Systems/Audio/AudioSystem.h"
#include "Systems/Cheat/ICheatSystem.h"
#include "Systems/Cheat/CheatSystem.h"
#include "Systems/Camera/ICameraSystem.h"
#include "Systems/Camera/CameraSystem.h"
#include "Systems/Component/IComponentFactory.h"
#include "Systems/Component/ComponentFactory.h"

using namespace gl;
using namespace RassEngine;
using namespace RassEngine::Systems;

static constexpr std::string_view DEFAULT_SCENE_NAME = "TestScene";

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow) {
	UNREFERENCED_PARAMETER(hInstance);
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

#ifdef _DEBUG
	// Setting up memory manager first
	auto memoryManager = std::make_unique<MemorySystem>();

	// Move console output to the pop-up
	AllocConsole();
	FILE *fp;
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONOUT$", "w", stderr);
	std::println("Debug Console Initialized...");
#endif

	// Register Memory System before all else, then
	// register Logging System after binding to the debug console,
	// but before the window is created
	auto builder = std::make_unique<Systems::SystemsManagerBuilder>();
	builder->Register<ILoggingSystem, LoggingSystem>();

	// Create a window instance
	std::shared_ptr<GLFWwindow> window = Engine::CreateGlfwWindow();
	if(!window) {
		LOG_ERROR("{} failed", NAMEOF(Engine::CreateGlfwWindow()));
		return -1;
	}

	// Bind OpenGL functions using glbinding, with the GLFW function loader
	glbinding::initialize(glfwGetProcAddress);

	// Register systems here, using the builder pattern
	(*builder)
		.Register<IGlobalEventsSystem, GlobalEventsSystem>()
		.Register<IRenderSystem, RenderSystemOpenGl>(window.get())
		.Register<IInputSystem, InputSystemGlfw>(window.get())
		.Register<ISceneSystem, SceneSystem>(DEFAULT_SCENE_NAME)
		.Register<IResourceSystem, ResourceSystem>()
		.Register<ITimeSystem, TimeSystem>()
		.Register<IPhysicsSystem, PhysicsSystem>()
		.Register<IImGuiSystem, ImGuiSystem>(window.get())
		.Register<IAudioSystem, AudioSystem>()
		.Register<IComponentFactory, ComponentFactory>()
		.Register<IEntityFactory, EntityFactory>()
	  .Register<ICameraSystem, CameraSystem>();
	
#ifdef _DEBUG
	// Only register the cheats if in debug mode
	std::initializer_list<std::string_view> scenePaths = {
		DEFAULT_SCENE_NAME
	};

	builder->Register<ICheatSystem, CheatSystem>(scenePaths);
	builder->Register<IDebugDrawSystem, DebugDrawSystem>();
#endif // _DEBUG

	Engine gameEngine{builder->build()};

	// Clean-up builder
	builder.reset(nullptr);

	// Make sure the game engine can run
	if(!gameEngine.canRun()) {
		LOG_ERROR("{} is not setup correctly", NAMEOF(RassEngine::Engine));
		glfwTerminate();
		return 1;
	}

	// Run the game
	gameEngine.run();
	return 0;
}
