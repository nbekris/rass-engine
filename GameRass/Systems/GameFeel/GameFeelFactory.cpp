// File Name:    GameFeelFactory.cpp
// Author(s):    main Steven Yacoub, secondary Taro Omiya, Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      System providing custom memory allocation.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include <Precompiled.h>
#include "GameFeelFactory.h"
#include "IGameFeelAction.h"

#include <algorithm>
#include <string_view>

#include <Entity.h>
#include <Events/Global.h>
#include <Events/SceneChange.h>
#include <Systems/GlobalEvents/IGlobalEventsSystem.h>
#include <Systems/Logging/ILoggingSystem.h>
#include <Systems/Scene/ISceneSystem.h>
#include <Systems/Time/ITimeSystem.h>
#include <Utils.h>


using namespace RassEngine;
using namespace RassEngine::Events;
using namespace RassEngine::Systems;

namespace RassGame::Systems {

GameFeelFactory::GameFeelFactory()
	: onSceneLoaded{this, &GameFeelFactory::OnSceneLoaded}
	, onSceneUnloaded{this, &GameFeelFactory::OnSceneUnloaded}
	, onUpdate{this, &GameFeelFactory::OnUpdate} {}

GameFeelFactory::~GameFeelFactory() {}

bool GameFeelFactory::Initialize() {
	if(!ISceneSystem::Get() || !IGlobalEventsSystem::Get()) {
		LOG_ERROR("{}: failed to initialize: missing scene/events system", NameClass());
		return false;
	}
	IGlobalEventsSystem::Get()->bind(SceneChange::AfterInitialize, &onSceneLoaded);
	IGlobalEventsSystem::Get()->bind(SceneChange::BeforeShutdown, &onSceneUnloaded);
	IGlobalEventsSystem::Get()->bind(Global::Update, &onUpdate);
	return true;
}
void GameFeelFactory::Shutdown() {
	if(!IGlobalEventsSystem::Get()) {
		return;
	}
	IGlobalEventsSystem::Get()->unbind(SceneChange::AfterInitialize, &onSceneLoaded);
	IGlobalEventsSystem::Get()->unbind(SceneChange::BeforeShutdown, &onSceneUnloaded);
	IGlobalEventsSystem::Get()->unbind(Global::Update, &onUpdate);
}

bool GameFeelFactory::Show(const glm::vec3 &color, const TweenCurve &curve) {
	return flashComponent ? flashComponent->Show(color, curve) : false;
}

bool GameFeelFactory::OnSceneLoaded(const IEvent<GlobalEventArgs> *, const GlobalEventArgs &) {
	// Make sure the scene system is available
	if(!ISceneSystem::Get()) {
		LOG_ERROR("{}: failed to find screen flasher: no scene system", NameClass());
		return false;
	}

	// Retrieve the screen flasher entity
	const Entity* screenFlashEntity = ISceneSystem::Get()->FindEntity(ENTITY_NAME);
	if(!screenFlashEntity) {
		LOG_WARNING("{}: failed to find screen flasher: entity \"{}\" not found", NameClass(), ENTITY_NAME);
		return false;
	}

	// Retrieve the screen flasher component
	flashComponent = screenFlashEntity->Get<Components::SpriteFader>();
	if(!flashComponent) {
		LOG_WARNING("{}: failed to find screen flasher: component \"{}\" not found", NameClass(), NAMEOF(RassEngine::Components::SpriteFader));
		return false;
	}
	return true;
}

bool GameFeelFactory::OnSceneUnloaded(const IEvent<GlobalEventArgs> *, const GlobalEventArgs &) {
	flashComponent = nullptr;
	activePlaybacks.clear();   // avoid dangling references to actions that may be destroyed with the scene
	return true;
}

void GameFeelFactory::RequestHitStop(float unscaledDuration, float timeScale) {
	auto *time = ITimeSystem::Get();
	if(time == nullptr || unscaledDuration <= 0.0f) {
		return;
	}
	if(!hitStopActive) {
		hitStopPrevScale = time->GetTimeScale();   // remember normal scale (usually 1.0)
	}
	hitStopActive = true;
	hitStopRemaining = unscaledDuration;           // refresh (re-trigger extends, not stacks)
	time->SetTimeScale(timeScale);
}

void GameFeelFactory::PlayActions(const std::vector<std::shared_ptr<IGameFeelAction>> &actions, const FeelContext &context) {
	if(actions.empty()) {
		return;
	}
	DetachedPlayback pb;
	pb.actions = actions;
	pb.context = context;
	activePlaybacks.push_back(std::move(pb));
}

bool GameFeelFactory::OnUpdate(const IEvent<GlobalEventArgs> *, const GlobalEventArgs &) {
	if(ITimeSystem::Get() == nullptr) {
		return true;
	}
	const float dt = ITimeSystem::Get()->GetUnscaledDeltaTime();   // hitstop will not freeze the timeline
	for(auto &pb : activePlaybacks) {
		pb.elapsed += dt;
		while(pb.cursor < pb.actions.size() && pb.actions[pb.cursor]->triggerTime <= pb.elapsed) {
			pb.actions[pb.cursor]->Execute(pb.context);
			++pb.cursor;
		}
	}
	std::erase_if(activePlaybacks, [] (const DetachedPlayback &pb) {
		return pb.cursor >= pb.actions.size();
		});

	if(hitStopActive) {
		hitStopRemaining -= dt;                    // dt is UNSCALED — will not freeze
		if(hitStopRemaining <= 0.0f) {
			hitStopActive = false;
			if(auto *time = ITimeSystem::Get()) {
				time->SetTimeScale(hitStopPrevScale);   // restore normal time
			}
		}
	}
	return true;
}

const std::string_view &GameFeelFactory::NameClass() const {
	static constexpr std::string_view className = NAMEOF(RassGame::Systems::GameFeelFactory);
	return className;
}
}
