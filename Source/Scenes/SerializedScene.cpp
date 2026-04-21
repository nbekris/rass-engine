// File Name:    SerializedScene.cpp
// Author(s):    main Taro Omiya, secondary Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Scene loaded from a serialized JSON file.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "SerializedScene.h"

#include <glm/vec3.hpp>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

#include "Components/Transform.h"
#include "Entity.h"
#include "Systems/Entity/IEntityFactory.h"
#include "Scene.h"
#include "Stream.h"
#include "Systems/Resource/IResourceSystem.h"
#include "Systems/Logging/ILoggingSystem.h"

using namespace RassEngine::Systems;

namespace RassEngine::Scenes {

static constexpr std::string_view KEY_SCENE = "Scene";
static constexpr std::string_view KEY_SCENE_NAME = "Name";
static constexpr std::string_view KEY_ENTITIES = "Entities";
static constexpr std::string_view KEY_ENTITY_PATH = "Path";
static constexpr std::string_view KEY_ENTITY_UUID = "UUID";
static constexpr std::string_view KEY_ENTITY_POSITION = "Position";
static constexpr std::string_view KEY_ENTITY_OVERRIDES = "Overrides";

SerializedScene::SerializedScene(const std::string_view &fileName)
	: Scene{fileName}, _fileName{fileName} {
	if(fileName.empty()) {
		throw std::invalid_argument("SerializedScene: file name cannot be empty");
	}
}

bool SerializedScene::Read(Stream &stream) {
	// Make sure argument is valid before reading
	if(!stream.IsValid()) {
		LOG_ERROR("{}: Invalid stream argument", NameClass());
		return false;
	} else if(!stream.Contains(KEY_SCENE)) {
		LOG_ERROR("{}: Stream does not contain a {} node", NameClass(), KEY_SCENE);
		return false;
	}

	// Start reading scene information
	stream.PushNode(KEY_SCENE);

	// Read the name of the scene
	stream.Read(KEY_SCENE_NAME, name);

	// Check if there are any entities to read
	if(!stream.Contains(KEY_ENTITIES)) {
		// If not, stop reading prematurely
		stream.PopNode(); // Pop KEY_SCENE
		return true;
	}

	// Otherwise, start reading each entity
	stream.ReadArray(KEY_ENTITIES, [this, &stream] () {
		// First, attempt to grab the file name of the entity
		std::string entityFileName;
		if(!stream.Read(KEY_ENTITY_PATH, entityFileName)) {
			LOG_WARNING("{}: Failed to read \"Path\" for an entity in the scene", NameClass());
			return;
		}

		// Read the entity's UUID
		UUID::Type entityUUID;
		std::unique_ptr<Entity> entity = stream.Read(KEY_ENTITY_UUID, entityUUID)
			// Construct the entity
			? IEntityFactory::Get()->Build(entityFileName, UUID(entityUUID))
			: IEntityFactory::Get()->Build(entityFileName);
		if(entity == nullptr) {
			LOG_WARNING("{}: Failed to construct entity, \"{}\"", NameClass(), entityFileName);
			return;
		}

		// Read the entity's position
		glm::vec3 entityPosition;
		if(stream.ReadVec3(KEY_ENTITY_POSITION, entityPosition)) {
			// If successful, set the entity's position
			Components::Transform *transform = entity->GetTransform();
			if(transform != nullptr) {
				transform->SetPosition(entityPosition);
			} else {
				LOG_WARNING("{}: Entity \"{}\" does not have a transform component, cannot set position", NameClass(), entityFileName);
			}
		}

		// Check if we want to override any of the entity's attributes
		if(stream.Contains(KEY_ENTITY_OVERRIDES)) {
			// Perform a normal read operation
			entity->Read(stream);
		}

		// Add the entity to the scene
		AddEntity(std::move(entity));
	});

	// We're done reading the list of entities
	stream.PopNode(); // Pop KEY_SCENE
	return true;
}

bool SerializedScene::Initialize() {
	// Determine the actual file path of the scene resource
	std::string filePath = IResourceSystem::Path(SCENE_FOLDER, _fileName, SCENE_EXTENSION);

	// First, construct a new stream from the file path
	Stream sceneStream = Stream(filePath);
	if(!sceneStream.IsValid()) {
		LOG_ERROR("{}: Failed to read scene from file path \"{}\"", NameClass(), filePath);
		return false;
	}

	// Read the stream
	if(!Read(sceneStream)) {
		LOG_ERROR("{}: Failed to read scene from stream", NameClass());
		return false;
	}

	// Otherwise, call the base method
	return Scene::Initialize();
}

const std::string_view &SerializedScene::NameClass() const {
	static constexpr std::string_view className = NAMEOF(Scenes::SerializedScene);
	return className;
}

}
