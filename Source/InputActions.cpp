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

}
