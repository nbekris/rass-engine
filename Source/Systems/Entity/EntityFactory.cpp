// File Name:    EntityFactory.cpp
// Author(s):    main Taro Omiya, secondary Steven Yacoub, Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Factory for creating entities from definitions.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "EntityFactory.h"

#include <memory>
#include <string>
#include <string_view>
#include <utility>

#include "Components/Transform.h"
#include "Entity.h"
#include "Stream.h"
#include "Systems/Resource/IResourceSystem.h"
#include "Utils.h"
#include "UUID.h"

namespace RassEngine::Systems {

std::unique_ptr<Entity> EntityFactory::Build(const std::string_view &entityName) {
	// Check if we already constructed this archetype
	const std::string name{entityName};
	if(name == DEFAULT_ENTITY_NAME) {
		return BuildEmpty();
	}

	auto it = archetypes.find(name);
	if(it != archetypes.end()) {
		// If so, clone the entity and return it
		return it->second->Clone();
	}

	// If not, read the JSON file
	std::string filePath = Systems::IResourceSystem::Path(DEFAULT_PATH, name, DEFAULT_EXTENSION);

	Stream entityStream = Stream(filePath);
	if(!entityStream.IsValid() || !entityStream.Contains(Entity::KEY_ENTITY)) {
		return nullptr;
	}

	// Construct the archetype
	std::unique_ptr<Entity> archetype = std::make_unique<Entity>();
	archetype->Read(entityStream);

	// Borrow the pointer
	Entity *borrowedPtr = archetype.get();

	// Store the archetype
	archetypes.emplace(name, std::move(archetype));

	// Clone the entity
	return borrowedPtr->Clone();
}

std::unique_ptr<Entity> EntityFactory::Build(const std::string_view &entityName, const UUID &entityID) {
	std::unique_ptr<Entity> toReturn = Build(entityName);
	toReturn->SetID(entityID);
	return std::move(toReturn);
}

std::unique_ptr<Entity> EntityFactory::BuildEmpty(const std::string_view &entityName) {
	// Check if we already constructed this archetype
	static const std::string EMPTY{DEFAULT_ENTITY_NAME};
	auto it = archetypes.find(EMPTY);
	if(it != archetypes.end()) {
		// If so, clone the entity
		std::unique_ptr<Entity> toReturn = it->second->Clone();

		// Change its name, then return it
		toReturn->Name(entityName);
		return std::move(toReturn);
	}

	// Construct the archetype
	std::unique_ptr<Entity> archetype = std::make_unique<Entity>();

	// Attach a transform component
	archetype->MakeComponent<Components::Transform>();

	// Borrow the pointer
	Entity *borrowedPtr = archetype.get();

	// Store the archetype
	archetypes.emplace(EMPTY, std::move(archetype));

	// Clone the entity
	std::unique_ptr<Entity> toReturn = borrowedPtr->Clone();

	// Change its name, then return it
	toReturn->Name(entityName);
	return std::move(toReturn);
}

void EntityFactory::Shutdown() {
	archetypes.clear();
}

bool EntityFactory::Initialize() {
	return true;
}

const std::string_view &EntityFactory::NameClass() const {
	static constexpr std::string_view className = NAMEOF(Systems::EntityFactory);
	return className;
}

}
