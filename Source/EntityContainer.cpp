// File Name:    EntityContainer.cpp
// Author(s):    main Niko Bekris, secondary Steven Yacoub, Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Container managing all active entities.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "EntityContainer.h"

#include <algorithm>
#include <utility>
#include <memory>
#include <vector>

#include "Entity.h"
#include "Systems/Logging/ILoggingSystem.h"

namespace RassEngine {
EntityContainer::EntityContainer()
	: container{}, nameToEntityCache{}, idToEntityCache{} {
	container.reserve(100);
	idToEntityCache.reserve(100);
	nameToEntityCache.reserve(100);
}

void EntityContainer::Add(std::unique_ptr<Entity> &&e) {
	if(e == nullptr) {
		LOG_WARNING("{}: Cannot add nullptr {}", NAMEOF(RassEngine::EntityContainer), NAMEOF(RassEngine::Entity));
		return;
	}

	auto idIter = idToEntityCache.find(e->GetID());
	if(idIter != idToEntityCache.end()) {
		LOG_WARNING("{}: Entity with ID '{}' already exists in the container", NAMEOF(RassEngine::EntityContainer), static_cast<UUID::Type>(e->GetID()));
		return;
	}

	// Add this new pointer into the cache
	AddToCache(e.get());

	// Move the unique_ptr into the container
	container.emplace_back(std::move(e));
}

void EntityContainer::Remove(const Entity *toRemove) {
	if(toRemove == nullptr) {
		LOG_WARNING("{}: Cannot remove nullptr {}", NAMEOF(RassEngine::EntityContainer), NAMEOF(RassEngine::Entity));
		return;
	}

	// Attempt to find this entity in the container
	const auto iter = std::remove_if(container.begin(), container.end()
		, [toRemove] (const std::unique_ptr<Entity> &entity) {
			return entity.get() == toRemove;
		}
	);
	if (iter == container.end()) {
		LOG_WARNING("{}: Cannot remove entity with ID '{}': not found in container", NAMEOF(RassEngine::EntityContainer), static_cast<UUID::Type>(toRemove->GetID()));
		return;
	}

	// Remove the entity from the cache and container
	RemoveFromCache(toRemove);
	container.erase(iter, container.end());
}

bool EntityContainer::CleanDestroyedEntities() {
	std::erase_if(container, [this] (const std::unique_ptr<Entity> &entity) {
		bool toReturn = entity->IsDestroyed();
		if(toReturn) {
			RemoveFromCache(entity.get());
		}
		return toReturn;
	});
	return true;
}

Entity *EntityContainer::Find(const UUID &id) const {
	auto it = idToEntityCache.find(id);
	return (it != idToEntityCache.end()) ? it->second : nullptr;
}

Entity *EntityContainer::Find(const std::string_view &name) const {
	const std::vector<Entity *> *results = FindAll(name);
	if((results == nullptr) || results->empty()) {
		return nullptr;
	}

	// Go throgh the results, and return the first valid entity
	for(auto &check : *results) {
		if(check && !check->IsDestroyed()) {
			return check;
		}
	}

	// Otherwise, return nullptr
	return nullptr;
}

const std::vector<Entity *> *EntityContainer::FindAll(const std::string_view &name) const {
	// Attempt to find this name in the cache
	auto iter = nameToEntityCache.find(std::string{name});
	if(iter != nameToEntityCache.end()) {
		// If found, return the cached vector
		return &iter->second;
	}

	return nullptr;
}

void EntityContainer::AddToCache(Entity *toAdd) {
	// Add this new pointer into the cache
	idToEntityCache.emplace(toAdd->GetID(), toAdd);

	// Check if this name is already cached
	std::string nameToAdd{toAdd->Name()};
	auto nameIter = nameToEntityCache.find(nameToAdd);
	if(nameIter != nameToEntityCache.end()) {
		// If so, append this new pointer into the cache
		nameIter->second.emplace_back(toAdd);
		return;
	}

	// Otherwise, create a new vector with this entity
	nameToEntityCache.emplace(nameToAdd, std::vector<Entity *>{toAdd});
}

void EntityContainer::RemoveFromCache(const Entity *toRemove) {
	// Remove this entity from the ID cache
	idToEntityCache.erase(toRemove->GetID());

	// Check if the name of this entity is cached
	auto iter = nameToEntityCache.find(std::string{toRemove->Name()});
	if(iter == nameToEntityCache.end()) {
		// If not, halt
		return;
	}

	// If this is the only entity with this name...
	if(iter->second.size() <= 1) {
		// Remove the entire entry
		nameToEntityCache.erase(iter->first);
	} else {
		// Otherwise, remove this entity from the vector
		std::erase(iter->second, toRemove);
	}
}

}
