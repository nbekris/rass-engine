// File Name:    Scene.cpp
// Author(s):    main Taro Omiya, secondary Steven Yacoub, Niko Bekris, Eric Fleegal
// Course:       GAM541
// Project:      RASS
// Purpose:      Base scene class.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "Scene.h"

#include <algorithm>

#include "Entity.h"
#include "Events/GlobalEventArgs.h"
#include "Events/Update.h"
#include "IEvent.h"
#include "Systems/GlobalEvents/GlobalEventsSystem.h"
#include "Systems/Logging/ILoggingSystem.h"

namespace RassEngine {

Scene::Scene(const std::string_view &name)
	: name{name}, entities{}
	, afterUpdateListener{this, &Scene::OnAfterUpdate}
{}

Scene::~Scene() {
	Shutdown();
}

bool Scene::Initialize() {
	if(Systems::GlobalEventsSystem::Get() == nullptr) {
		return false;
	}

	// Bind to the global After-Update event
	Systems::GlobalEventsSystem::Get()->bind(Events::Update::After, &afterUpdateListener);

	// Initialize all entities
	bool toReturn = true;
	std::for_each(entities.begin(), entities.end(), [this, &toReturn] (Entity &entity) {
		// Intialize this entity, and combine the result with the overall initialization result
		toReturn &= entity.Initialize();
	});

	// Indicate we're done initializing
	isInitialized = true;
	return toReturn;
}

bool Scene::Shutdown() {
	if(Systems::GlobalEventsSystem::Get() != nullptr) {
		Systems::GlobalEventsSystem::Get()->unbind(Events::Update::After, &afterUpdateListener);
	}

	// Empty the entity list
	std::for_each(entities.begin(), entities.end(), [] (Entity &entity) {
		entity.Destroy();
	});
	return entities.CleanDestroyedEntities();
}

bool Scene::OnAfterUpdate(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &) {
	// Remove entities marked as destroyed
	return entities.CleanDestroyedEntities();
}

}
