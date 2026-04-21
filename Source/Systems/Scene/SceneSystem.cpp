// File Name:    SceneSystem.cpp
// Author(s):    main Taro Omiya, secondary Boyuan Hu, Niko Bekris, Steven Yacoub, Eric Fleegal
// Course:       GAM541
// Project:      RASS
// Purpose:      System managing scene loading and transitions.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "SceneSystem.h"

#include <algorithm>
#include <functional>
#include <memory>
#include <string_view>
#include <typeinfo>
#include <utility>

#include "Entity.h"
#include "Events/GlobalEventArgs.h"
#include "Events/SceneChange.h"
#include "Events/Update.h"
#include "IEvent.h"
#include "Scene.h"
#include "Scenes/SerializedScene.h"
#include "Systems/GlobalEvents/IGlobalEventsSystem.h"
#include "Systems/Logging/ILoggingSystem.h"

namespace RassEngine::Systems {

SceneSystem::SceneSystem(const std::string_view &firstSceneName)
	: pendingScene{std::make_unique<Scenes::SerializedScene>(firstSceneName)}
	, updateListener{this, &SceneSystem::OnUpdate} {}

SceneSystem::~SceneSystem() {
	Shutdown();
}

bool SceneSystem::Initialize() {
	if(IGlobalEventsSystem::Get() == nullptr) {
		return false;
	}

	// Bind to all events
	IGlobalEventsSystem::Get()->bind(Events::Update::After, &updateListener);
	return true;
}

void SceneSystem::Shutdown() {
	// Make sure the events are available
	if(IGlobalEventsSystem::Get() == nullptr) {
		return;
	}

	// Unbind to all events
	IGlobalEventsSystem::Get()->unbind(Events::Update::After, &updateListener);
}

void SceneSystem::AddEntity(std::unique_ptr<Entity> &&entity) {
	// Make sure argument is valid
	if(entity == nullptr) {
		LOG_WARNING("{}: Cannot add nullptr {} to {}", NameClass(), NAMEOF(RassEngine::Entity), NAMEOF(RassEngine::Scene));
		return;
	}

	// Add the entity to the scene
	Entity *toInit = entity.get();
	activeScene->AddEntity(std::move(entity));

	// If the scene is already initialized, initialize this entity immediately.
	if(activeScene->IsReady()) {
		toInit->Initialize();
	}
}

void SceneSystem::EndScene() {
	isEnding = true;
}

void SceneSystem::Restart() {
	isRestarting = true;
}

bool SceneSystem::IsRunning() const {
	return (activeScene || pendingScene);
}

bool SceneSystem::IsSceneChanging() const {
	return (isEnding || isRestarting || (pendingScene.get() != nullptr));
}

bool SceneSystem::IsSceneRestarting() const {
	return isRestarting;
}

Scene *SceneSystem::GetCurrentScene() const {
	return activeScene.get();
}

void SceneSystem::ForEachActiveEntity(std::function<void(const Entity *)> callback) const {
	if(!activeScene) {
		return;
	}

	// Get entities from the scene
	const EntityContainer &entities = activeScene->GetEntities();
	std::for_each(entities.cbegin(), entities.cend(), [&callback] (const Entity &entity) {
		// Invoke callback for each valid entity
		if(!entity.IsDestroyed()) {
			callback(&entity);
		}
	});
}

Entity *SceneSystem::FindEntity(const std::string_view &name) const {
	return (activeScene != nullptr) ? activeScene->FindEntity(name) : nullptr;
}

Entity *SceneSystem::FindEntity(const UUID &id) const {
	return (activeScene != nullptr) ? activeScene->FindEntity(id) : nullptr;
}

const std::vector<Entity *> *SceneSystem::FindEntities(const std::string_view &name) const {
	return (activeScene != nullptr) ? activeScene->FindEntities(name) : nullptr;
}

void SceneSystem::SetPendingScene(const std::type_info &sceneType, std::function<std::unique_ptr<Scene>()> sceneGenerator) {
	// Check if there is a scene already loaded
	if(activeScene == nullptr) {
		// If not, construct a new pending scene
		pendingScene = sceneGenerator();

		// Wait for the next frame to load this scene
		return;
	}

	// Check if the new pending scene type differs from the active one
	if(typeid(*activeScene) != sceneType) {
		// If so, construct a new pending scene
		pendingScene = sceneGenerator();

		// Wait for the next frame to load this scene
		return;
	}

	// Check scene type
	if(typeid(Scenes::SerializedScene) == sceneType) {
		// For SerializedScene, constructing a scene is low-cost.
		std::unique_ptr<Scene> newScene = sceneGenerator();

		// Compare the new scene's file path with the current active one
		Scenes::SerializedScene *currentScene = dynamic_cast<Scenes::SerializedScene *>(activeScene.get())
			, *newScenePtr = dynamic_cast<Scenes::SerializedScene *>(newScene.get());
		if((currentScene == nullptr) || (currentScene->Name() != newScenePtr->Name())) {
			// If file paths differ, or the current scene is not a SerializedScene,
			// then load the new scene as pending.
			pendingScene = std::move(newScene);

			// Wait for the next frame to load this scene
			return;
		}
	}

	// Otherwise, just restart the current scene.
	// Clear any existing pending scene to avoid an unintended scene change after restart.
	pendingScene.reset();
	Restart();
}
bool SceneSystem::OnUpdate(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &) {
	// Check for a scene change.
	if(IsSceneChanging()) {
		// Shutdown the active scene.
		CallShutdown();

		if(IsSceneRestarting()) {
			ClearRestartFlag();
		} else {
			// Change the active scene.
			// [NOTE: Any existing active scene is automatically deleted.]
			activeScene = pendingScene != nullptr ? std::move(pendingScene) : nullptr;
		}

		// Initialize the new active scene.
		CallInitialize();
	}
	return true;
}

void SceneSystem::CallInitialize() const {
	if(activeScene == nullptr) {
		return;
	}

	// Indicate we're about to initialize
	IGlobalEventsSystem::Get()->call(Events::SceneChange::BeforeInitialize);

	// Call LoggingSystem::Verbose, passing the format string "{}: Initialize" and the name of the active scene.
	LOG_INFO("{}: Initialize", activeScene->Name());
	activeScene->Initialize();

	// Indicate we finished initializing
	IGlobalEventsSystem::Get()->call(Events::SceneChange::AfterInitialize);
}

void SceneSystem::CallShutdown() const {
	if(activeScene == nullptr) {
		return;
	}

	// Indicate we're about to shutdown
	IGlobalEventsSystem::Get()->call(Events::SceneChange::BeforeShutdown);

	// Call LoggingSystem::Verbose, passing the format string "{}: Shutdown" and the name of the active scene.
	LOG_INFO("{}: Shutdown", activeScene->Name());
	activeScene->Shutdown();

	// Indicate we finished shutting down
	IGlobalEventsSystem::Get()->call(Events::SceneChange::AfterShutdown);
}

const std::string_view &SceneSystem::NameClass() const {
	static constexpr std::string_view className = NAMEOF(RassEngine::Systems::SceneSystem);
	return className;
}

}
