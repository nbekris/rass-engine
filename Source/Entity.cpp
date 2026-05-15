// File Name:    Entity.cpp
// Author(s):    main Taro Omiya, secondary Steven Yacoub, Niko Bekris, Boyuan Hu, Eric Fleegal
// Course:       GAM541
// Project:      RASS
// Purpose:      Game entity base class.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "Entity.h"

#include <utility>

#include "Component.h"
#include "Components/Transform.h"
#include "Stream.h"
#include "Systems/Logging/ILoggingSystem.h"
#include "Systems/Component/IComponentFactory.h"

namespace RassEngine {

static constexpr std::string_view KEY_OVERRIDES = "Overrides";
static constexpr size_t STARTING_COMPONENTS_CAPACITY = 3;

Entity::Entity() : Object{}
, events{} {
	// Add padding for components
	components.reserve(STARTING_COMPONENTS_CAPACITY);
}

Entity::Entity(const Entity &other) : Object{other}
, name{other.name}, isDestroyed{other.isDestroyed}
, components{}, events{} {
	components.reserve(other.components.size());
	nameToComponentCache.reserve(other.components.size());
	for(const std::unique_ptr<Component> &component : other.components) {
		AddComponent(component->Clone());
	}
}

Entity::~Entity(void) {
	nameToComponentCache.clear();
	components.clear();
}

bool Entity::Initialize() {
	// Only return true if all components initialize successfully
	for(auto& component : components) {
		if(!component->Initialize()) {
			return false;
		}
	}
	return true;
}

const std::string_view &Entity::NameClass() const {
	static constexpr std::string_view className = NAMEOF(Entity);
	return className;
}

std::unique_ptr<Entity> Entity::Clone() const {
	return std::make_unique<Entity>(*this);
}

void Entity::AddComponent(std::unique_ptr<Component>&& component) {
	// Make sure argument is valid
	if(component == nullptr) {
		LOG_WARNING("{}: Cannot add nullptr {} to {}", NameClass(), NAMEOF(RassEngine::Component), NAMEOF(RassEngine::Entity));
		return;
	}

	// Update the component's parent pointer
	component->Parent(this);

	// Add component into the components list
	Component *borrowedPtr = component.get();
	components.emplace_back(std::move(component));

	// Also add component to the cache
	std::string className = Utils::RemoveNamespace(borrowedPtr->NameClass());
	const auto cacheIter = nameToComponentCache.find(className);
	if(cacheIter != nameToComponentCache.end()) {
		cacheIter->second.emplace_back(borrowedPtr);
		return;
	}

	// Otherwise, add a new entry into the cache
	nameToComponentCache.emplace(className, std::vector<Component *>{borrowedPtr});
}

void Entity::Destroy() {
	// Mark this entity as destroyed
	isDestroyed = true;

	// Check if this entity has a transform
	Components::Transform *toEval = GetTransform();
	if(toEval == nullptr) {
		return;
	}

	// Go through all transform's children
	for(const Components::Transform *child : *toEval) {
		if(child != nullptr) {
			// Mark this child's entity to be destroyed as well.
			child->Parent()->Destroy();
		}
	}
}

Components::Transform *Entity::GetTransform() const {
	if(cacheTransform == nullptr) {
		cacheTransform = Get<Components::Transform>();
	}
	return cacheTransform;
}

Component *Entity::ReadNewComponent(Stream &stream, const std::string &componentName, const std::string_view &jsonKey) {
	// Retrieve the factory
	auto *factory = Systems::IComponentFactory::Get();
	if(factory == nullptr) {
		LOG_WARNING("{}: {} is not registered", NAMEOF(Entity), NAMEOF(Systems::IComponentFactory));
		return nullptr;
	}

	// Create the component
	Component* comp = factory->Create(componentName);
	if(comp == nullptr) {
		LOG_WARNING("{}: No registered component of type: {}", NAMEOF(Entity), componentName);
		return nullptr;
	}

	// Update the component's parent pointer
	comp->Parent(this);

	// Figure out what the node's key is
	std::string jsonNodeKey;
	if(!jsonKey.empty()) {
		jsonNodeKey = std::string(jsonKey);
	} else {
		jsonNodeKey = componentName;
	}

	// Push the component's object onto stream
	bool isPushed = stream.PushNode(jsonNodeKey);
	try {
		// Read from the component's stream
		comp->Read(stream);

		// Add the component
		AddComponent(std::unique_ptr<Component>{comp});
	} catch (std::exception ex) {
		// Print errors
		LOG_ERROR("{}: could not read a new component from key \"{}\":\n{}", NAMEOF(Entity), jsonNodeKey, ex.what());

		// Delete the component
		delete comp;
		comp = nullptr;
	}

	if(isPushed) {
		// Pop the component's object onto stream
		stream.PopNode();
	}

	return comp;
}

void Entity::ReadOverrideComponent(Stream &stream, const std::string &componentName) {
	// Search component by key, and override it if it exists
	auto componentIter = nameToComponentCache.find(componentName);
	if(componentIter == nameToComponentCache.end()) {
		LOG_WARNING("{}: No existing component of type {} to override", NameClass(), componentName);
		return;
	} else if(componentIter->second.size() < 1) {
		LOG_WARNING("{}: No existing component of type {} to override", NameClass(), componentName);
		return;
	} else if(componentIter->second.size() > 1) {
		LOG_WARNING("{}: Multiple existing components of type {} to override; only overriding the first one", NameClass(), componentName);
	}

	// If found, for now, override the values of the first component
	stream.PushNode(componentName);
	componentIter->second[0]->Read(stream);
	stream.PopNode();
}

void Entity::SetID(const UUID &newID) {
	id = newID;
}

bool Entity::Read(Stream& stream) {
	// Make sure argument is valid before reading
	if(!stream.IsValid()) {
		LOG_ERROR("{}: Invalid stream argument", NameClass());
		return false;
	}

	// In override mode, the entity and its components
	// are already created: we just need to override its attributes
	// Defaulting to false, just create a new component each time.
	bool isOverriding = false;

	// Check what *operation* we're performing, here
	if(stream.Contains(KEY_ENTITY)) {
		// We're reading the stream for creating a new entity.
		// Push the entity node, accordingly.
		stream.PushNode(KEY_ENTITY);
	} else if(stream.Contains(KEY_OVERRIDES)) {
		// If we're overriding, flag as such.
		isOverriding = true;

		// Push the override node, and proceed normally
		stream.PushNode(KEY_OVERRIDES);
	} else {
		LOG_ERROR("{}: Stream does not contain an Entity node", NameClass());
		return false;
	}

	stream.Read("Name", name);

	// Check if there are any components to read
	if(!stream.Contains("Components")) {
		// If not, stop reading prematurely
		stream.PopNode();
		return true;
	}

	// Otherwise, start reading each component via a lambda
	stream.ReadObject("Components", [this, &isOverriding, &stream] (const std::string &key) {
		if (!isOverriding) {
			ReadNewComponent(stream, key);
			return;
		}

		ReadOverrideComponent(stream, key);
	});


	// We're done reading components
	stream.PopNode(); // Pop "Entity"
	return true;
}

}
