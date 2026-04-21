// File Name:    SerializedScene.h
// Author(s):    main Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Scene loaded from a serialized JSON file.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <string>
#include <string_view>

#include "ISerializable.h"
#include "Scene.h"
#include "Stream.h"

namespace RassEngine::Scenes {

class SerializedScene : public Scene, public ISerializable<Stream> {
	static constexpr std::string_view SCENE_FOLDER = "Scenes";
	static constexpr std::string_view SCENE_EXTENSION = "json";
public:
	SerializedScene(const std::string_view &fileName);
	inline virtual ~SerializedScene() override = default;

	// Inherited via Scene
	bool Initialize() override;
	const std::string_view &NameClass() const override;

	// Inherited via ISerializable
	bool Read(Stream &) override;
private:
	std::string _fileName;
};

}
