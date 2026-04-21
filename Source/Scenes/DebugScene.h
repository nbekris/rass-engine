// File Name:    DebugScene.h
// Author(s):    main Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Debug scene for development and testing.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <string_view>

#include "Scene.h"

namespace RassEngine::Scenes {

class DebugScene : public Scene {
public:
	inline DebugScene() : Scene{"DebugScene"} {}
	inline virtual ~DebugScene() override = default;

	// Inherited via Object
	bool Initialize() override;
	virtual const std::string_view &NameClass() const override;
};

}
