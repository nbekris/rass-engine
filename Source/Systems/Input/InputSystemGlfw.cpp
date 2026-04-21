// File Name:    InputSystemGlfw.cpp
// Author(s):    main Taro Omiya, secondary Niko Bekris, Boyuan Hu
// Course:       GAM541
// Project:      RASS
// Purpose:      System processing keyboard, mouse, and gamepad input via GLFW.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "InputSystemGlfw.h"

#include <cstring>
#include <stdexcept>
#include <GLFW/glfw3.h>

#include "Events/GlobalEventArgs.h"
#include "Events/Update.h"
#include "Events/Window.h"
#include "Systems/GlobalEvents/IGlobalEventsSystem.h"
#include "Systems/Logging/ILoggingSystem.h"
#include "Systems/Render/IRenderSystem.h"
#include "Systems/Scene/ISceneSystem.h"
#include "Utils.h"

#ifndef _DEBUG
#include "Systems/ImGui/IImGuiSystem.h"
#endif

namespace RassEngine::Systems {

InputSystemGlfw::InputSystemGlfw(GLFWwindow *window)
	: window{window}, currentKeys{0}, previousKeys{0}, mousePosRaw{}, mousePosViewport{}, currentMouseButtons{0}, previousMouseButtons{0}
	, updateListener{this, &InputSystemGlfw::Update} {
	if(window == nullptr) {
		throw std::invalid_argument("window cannot be null");
	}
}

InputSystemGlfw::~InputSystemGlfw() {
	Shutdown();
}

bool InputSystemGlfw::Initialize() {
	if(IGlobalEventsSystem::Get() == nullptr) {
		return false;
	}

	// Setup the keys
	std::memset(currentKeys, 0, sizeof(currentKeys));
	std::memset(previousKeys, 0, sizeof(previousKeys));

	// Setup mouse positions
	mousePosRaw = glm::dvec2(0.0, 0.0);
	mousePosViewport = glm::vec2(0.f, 0.f);

	// Setup mouse buttons
	currentMouseButtons = static_cast<std::byte>(0);
	previousMouseButtons = static_cast<std::byte>(0);

	// Bind to mouse button changes
	// First, by storing this system as user pointer
	glfwSetWindowUserPointer(window, this);
	// Then on C-based callback, cast the user pointer
	// and direct the arguments to the event method
	glfwSetMouseButtonCallback(window, [] (GLFWwindow *window, int button, int action, int) {
		static_cast<InputSystemGlfw *>(glfwGetWindowUserPointer(window))
			->OnMouseButtonChanged(button, action);
	});

	// Bind to callback when the window is iconified (minimized)
	glfwSetWindowIconifyCallback(window, [] (GLFWwindow *window, int iconified) {
		if(iconified && (IGlobalEventsSystem::Get() != nullptr)) {
			IGlobalEventsSystem::Get()->call(Events::Window::Minimize);
		}
	});

	// Bind to callback when the window loses focus
	glfwSetWindowFocusCallback(window, [] (GLFWwindow *window, int focused) {
		if(!focused && (IGlobalEventsSystem::Get() != nullptr)) {
			IGlobalEventsSystem::Get()->call(Events::Window::LoseFocus);
		}
	});

	// bind to the update event
	IGlobalEventsSystem::Get()->bind(Events::Update::Before, &updateListener);
	return true;
}

void RassEngine::Systems::InputSystemGlfw::Shutdown() {
	if(IGlobalEventsSystem::Get() != nullptr) {
		// Unbind from all events
		IGlobalEventsSystem::Get()->unbind(Events::Update::Before, &updateListener);
	}

	// Unbind the mouse from the game window
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// Unregister the mouse button callback and clear the user pointer
	glfwSetMouseButtonCallback(window, nullptr);
	glfwSetWindowUserPointer(window, nullptr);
	glfwSetWindowIconifyCallback(window, nullptr);
	glfwSetWindowFocusCallback(window, nullptr);
}

bool RassEngine::Systems::InputSystemGlfw::IsKeyDown(unsigned short key) const {
	if(key > GLFW_KEY_LAST) {
		return false;
	}
	return currentKeys[key];
}

bool RassEngine::Systems::InputSystemGlfw::IsKeyPressed(unsigned short key) const {
	if(key > GLFW_KEY_LAST) {
		return false;
	}
	return currentKeys[key] && !previousKeys[key];
}

bool InputSystemGlfw::IsKeyReleased(unsigned short key) const {
	if(key > GLFW_KEY_LAST) {
		return false;
	}
	return !currentKeys[key] && previousKeys[key];
}

bool InputSystemGlfw::Update(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &) {
	// Check if we closed the window
	if(glfwWindowShouldClose(window) && ISceneSystem::Get()) {
		// Quit the game
		ISceneSystem::Get()->EndScene();

		// Indicate success
		return true;
	}

#ifdef _DEBUG
	// In debug mode, P key quits the application
	if(IsKeyPressed(GLFW_KEY_P) && ISceneSystem::Get()) {
		ISceneSystem::Get()->EndScene();
		return true;
	}
#endif

#ifndef _DEBUG
	// Check whether we want to capture the mouse cursor
	if(!isMouseCaptured) {
		// First, check if ImGUI is null (we're in release mode) or ready
		auto *imguiSystem = IImGuiSystem::Get();
		if(!imguiSystem || imguiSystem->IsReady()) {
			if (imguiSystem) {
				// Then prevent IMGUI from overriding the mouse cursor behavior, if available
				imguiSystem->AllowCursorOverride(false);
			}

			// Capture the mouse to only stay within the game window
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			isMouseCaptured = true;
		}
	}
#endif //!_DEBUG

	// Copy over keys from last frame
	std::memcpy(previousKeys, currentKeys, sizeof(currentKeys));

	// Copy the current button states over the previous one
	previousMouseButtons = currentMouseButtons;

	// Poll GLFW events
	glfwPollEvents();

	// Poll for any keyboard input
	UpdateKeyChanges();

	// Poll the mouse cursor position
	UpdateMousePositions();

	return true;
}

void InputSystemGlfw::UpdateKeyChanges() {
	// Update key states
	for(int key = GLFW_KEY_SPACE; key <= GLFW_KEY_LAST; ++key) {
		int state = glfwGetKey(window, key);
		currentKeys[key] = (state == GLFW_PRESS || state == GLFW_REPEAT);

		// Print key name to console when pressed
		if(currentKeys[key] && !previousKeys[key]) {
			const char *name = glfwGetKeyName(key, 0);
			if(name) {
				LOG_INFO("Key pressed: {}", name);
			} else {
				LOG_INFO("Key pressed: {}", key);
			}
		}
	}
}

static float ConvertToRatio(double position, int span) {
	if(span <= 0) {
		// avoid divide-by-zero error
		return 0;
	}

	// Convert position from 0 - 1 range
	double toReturn = position / static_cast<double>(span);

	// Then to 0 - 2
	toReturn *= 2.0;
	// And finally, -1 - 1
	toReturn -= 1.0;
	return static_cast<float>(toReturn);
}

void InputSystemGlfw::UpdateMousePositions() {
	// Poll the mouse cursor position
	glfwGetCursorPos(window, &mousePosRaw.x, &mousePosRaw.y);

	// Default viewport positions to center of the screen
	mousePosViewport.x = 0.f;
	mousePosViewport.y = 0.f;
	if(auto *renderer = IRenderSystem::Get()) {
		// Normalize the mouse position to a range of -1 - 1
		// by the viewport's dimensions
		mousePosViewport.x = ConvertToRatio(mousePosRaw.x, renderer->getScreenWidth());

		// Flipping the sign, here, so (1, 1) indicates the upper-right hand corner
		mousePosViewport.y = -ConvertToRatio(mousePosRaw.y, renderer->getScreenHeight());
	}
}

static std::byte GetBitToSet(int button) {
	return static_cast<std::byte>(1 << button);
}
void InputSystemGlfw::OnMouseButtonChanged(int button, int action) {
	// Check if the mouse button are the ones we're concerned with
	switch(button) {
	case GLFW_MOUSE_BUTTON_LEFT:
	case GLFW_MOUSE_BUTTON_RIGHT:
	case GLFW_MOUSE_BUTTON_MIDDLE:
		break;
	default:
		return;
	}

	// Check if the button was pressed
	if(action == GLFW_PRESS) {
		// If so, flip this bit
		currentMouseButtons |= GetBitToSet(button);
	} else {
		// Otherwise, turn off the bit
		currentMouseButtons &= ~GetBitToSet(button);
	}
}

void InputSystemGlfw::WhenMouseButtonDown(unsigned short button, bool &thisFrame, bool &lastFrame) const {
	// Setup the bit to toggle
	std::byte bitToSet = GetBitToSet(button);

	// Check if bit is flipped in previous and current flags
	lastFrame = (previousMouseButtons & bitToSet) != static_cast<std::byte>(0);
	thisFrame = (currentMouseButtons & bitToSet) != static_cast<std::byte>(0);
}

bool InputSystemGlfw::IsMouseButtonDown(unsigned short button) const {
	// Check whether the button is held down this frame
	bool thisFrame, lastFrame;
	WhenMouseButtonDown(button, thisFrame, lastFrame);
	return thisFrame;
}

bool InputSystemGlfw::IsMouseButtonClicked(unsigned short button) const {
	// Only return true if button was clicked *this frame*
	bool thisFrame, lastFrame;
	WhenMouseButtonDown(button, thisFrame, lastFrame);
	return !lastFrame && thisFrame;
}

bool InputSystemGlfw::IsMouseButtonReleased(unsigned short button) const {
	// Only return true if button was released *this frame*
	bool thisFrame, lastFrame;
	WhenMouseButtonDown(button, thisFrame, lastFrame);
	return lastFrame && !thisFrame;
}

const std::string_view &InputSystemGlfw::NameClass() const {
	static constexpr std::string_view className = NAMEOF(InputSystemGlfw);
	return className;
}

}
