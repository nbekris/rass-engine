// File Name:    DemoScene.h
// Author(s):    main Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Demo scene showcasing engine features.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <string_view>

#include "Scene.h"

namespace RassEngine::Scenes {

class DemoScene : public Scene {
public:
	inline DemoScene() : Scene("DemoScene") {}
	inline virtual ~DemoScene() override = default;

	// Inherited via Object
	bool Initialize() override;
	virtual const std::string_view &NameClass() const override;
};

}
