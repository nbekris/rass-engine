// File Name:    InputActions.cpp
// Author(s):    main Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Defines and maps player input actions.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "InputActions.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <Systems/Input/IInputSystem.h>

using namespace RassEngine;
using namespace RassEngine::Systems;

namespace RassEngine {

bool InputActions::IsFlipPressed() {
	// Make sure input is available
	const IInputSystem *inputSystem = IInputSystem::Get();
	if(inputSystem == nullptr) {
		return false;
	}

	// Check the key presses
	return inputSystem->IsKeyPressed(GLFW_KEY_LEFT_SHIFT)
		|| inputSystem->IsKeyPressed(GLFW_KEY_RIGHT_SHIFT)
		|| inputSystem->IsKeyPressed(GLFW_KEY_DOWN);
}

bool InputActions::IsGrabBoxHeld() {
	// Make sure input is available
	const IInputSystem *inputSystem = IInputSystem::Get();
	if(inputSystem == nullptr) {
		return false;
	}

	// Check the key down
	return inputSystem->IsKeyDown(GLFW_KEY_LEFT_SHIFT)
		|| inputSystem->IsKeyDown(GLFW_KEY_RIGHT_SHIFT)
		|| inputSystem->IsKeyDown(GLFW_KEY_DOWN);
}

static constexpr float GAMEPAD_DEADZONE = 0.2f;

bool InputActions::IsMoveLeftHeld() {
	const IInputSystem *inputSystem = IInputSystem::Get();
	if(inputSystem == nullptr) {
		return false;
	}

	return inputSystem->IsKeyDown(GLFW_KEY_LEFT)
		|| inputSystem->IsKeyDown(GLFW_KEY_A)
		|| inputSystem->IsGamepadButtonDown(GLFW_GAMEPAD_BUTTON_DPAD_LEFT)
		|| inputSystem->GetGamepadAxis(GLFW_GAMEPAD_AXIS_LEFT_X) < -GAMEPAD_DEADZONE;
}

bool InputActions::IsMoveRightHeld() {
	const IInputSystem *inputSystem = IInputSystem::Get();
	if(inputSystem == nullptr) {
		return false;
	}

	return inputSystem->IsKeyDown(GLFW_KEY_RIGHT)
		|| inputSystem->IsKeyDown(GLFW_KEY_D)
		|| inputSystem->IsGamepadButtonDown(GLFW_GAMEPAD_BUTTON_DPAD_RIGHT)
		|| inputSystem->GetGamepadAxis(GLFW_GAMEPAD_AXIS_LEFT_X) > GAMEPAD_DEADZONE;
}

bool InputActions::IsJumpPressed() {
	const IInputSystem *inputSystem = IInputSystem::Get();
	if(inputSystem == nullptr) {
		return false;
	}

	return inputSystem->IsKeyPressed(GLFW_KEY_X)
		|| inputSystem->IsKeyPressed(GLFW_KEY_SPACE)
		|| inputSystem->IsKeyPressed(GLFW_KEY_UP)
		|| inputSystem->IsKeyPressed(GLFW_KEY_W)
		|| inputSystem->IsGamepadButtonPressed(GLFW_GAMEPAD_BUTTON_LEFT_BUMPER);
}

bool InputActions::IsJumpHeld() {
	const IInputSystem *inputSystem = IInputSystem::Get();
	if(inputSystem == nullptr) {
		return false;
	}

	return inputSystem->IsKeyDown(GLFW_KEY_X)
		|| inputSystem->IsKeyDown(GLFW_KEY_SPACE)
		|| inputSystem->IsKeyDown(GLFW_KEY_UP)
		|| inputSystem->IsKeyDown(GLFW_KEY_W)
		|| inputSystem->IsGamepadButtonDown(GLFW_GAMEPAD_BUTTON_LEFT_BUMPER);
}

}
