// File Name:    DebugScene.cpp
// Author(s):    main Taro Omiya, secondary Steven Yacoub, Niko Bekris, Eric Fleegal
// Course:       GAM541
// Project:      RASS
// Purpose:      Debug scene for development and testing.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "DebugScene.h"

#include <utility>
#include <memory>

#include "Systems/Entity/IEntityFactory.h"
#include "Stream.h"

namespace RassEngine::Scenes {

static constexpr std::initializer_list<std::string_view> DEBUG_ENTITIES = {
	"Background"
	, "TextEntity"
	, "HeroBeige"
	, "HeroGreen"
	, "HeroPink"
	, "HeroPurple"
	, "HeroYellow"
	, "Barnacle"
	, "Bee"
	, "Fish"
	, "Frog"
	, "Ladybug"
	, "Mouse"
	, "Saw"
};

bool DebugScene::Initialize() {

	for (const std::string_view &dataName : DEBUG_ENTITIES) {
		// Create an entity from the JSON file
		std::unique_ptr<Entity> newEntity = Systems::IEntityFactory::Get()->Build(dataName);

		// Add the entity into the scene
		AddEntity(std::move(newEntity));
	}

	// Call the base method
	return Scene::Initialize();
}

const std::string_view &DebugScene::NameClass() const {
	static constexpr std::string_view className = NAMEOF(Scenes::DebugScene);
	return className;
}

}
