// File Name:    InputActions.h
// Author(s):    main Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Defines and maps player input actions.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

namespace RassEngine {

struct InputActions {
	static bool IsFlipPressed();
	static bool IsGrabBoxHeld();
};

}
