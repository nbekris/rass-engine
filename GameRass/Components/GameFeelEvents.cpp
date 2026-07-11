#include "pch.h"
#include "GameFeelEvents.h"

#include <algorithm>
#include <memory>
#include <string>
#include <string_view>

#include <Entity.h>
#include <IEvent.h>
#include <Stream.h>
#include <Utils.h>
#include <Events/Global.h>
#include <Events/GlobalEventArgs.h>
#include <Systems/GlobalEvents/IGlobalEventsSystem.h>
#include <Systems/Time/ITimeSystem.h>
#include <Systems/Logging/ILoggingSystem.h>
#include <Components/Transform.h>

#include "../Systems/GameFeel/IGameFeelAction.h"
#include "../Systems/GameFeel/GameFeelActionFactory.h"
#include "../Systems/GameFeel/IGameFeelFactory.h"

using namespace RassEngine;
using namespace RassEngine::Events;
using namespace RassEngine::Systems;
using namespace RassGame::Systems;

namespace RassGame::Components {

GameFeelEvents::GameFeelEvents()
	: Cloneable<Component, GameFeelEvents>{}
	, onUpdateListener{this, &GameFeelEvents::Update} {}

GameFeelEvents::GameFeelEvents(const GameFeelEvents &other)
	: Cloneable<Component, GameFeelEvents>{other}
	, allActionLists{other.allActionLists}
	, onUpdateListener{this, &GameFeelEvents::Update} {}

GameFeelEvents::~GameFeelEvents() {
	if(IGlobalEventsSystem::Get() != nullptr) {
		IGlobalEventsSystem::Get()->unbind(Global::Update, &onUpdateListener);
	}
	isInitialized = false;
}

bool GameFeelEvents::Initialize() {
	if(Parent() == nullptr) {
		LOG_ERROR("{}: Parent {} not found", NameClass(), NAMEOF(Entity));
		return false;
	}
	if(IGlobalEventsSystem::Get() != nullptr) {
		IGlobalEventsSystem::Get()->bind(Global::Update, &onUpdateListener);
	}
	isInitialized = true;
	return true;
}

bool GameFeelEvents::Update(const IEvent<GlobalEventArgs> *, const GlobalEventArgs &) {
	if(ITimeSystem::Get() == nullptr) {
		return true;
	}
	// Unscaled: a hit-stop action setting timeScale=0 must not freeze the timeline.
	const float dt = ITimeSystem::Get()->GetUnscaledDeltaTime();

	for(auto &pb : activePlaybacks) {
		pb.elapsed += dt;
		const auto &actions = pb.list->actions;
		while(pb.cursor < actions.size() && actions[pb.cursor]->triggerTime <= pb.elapsed) {
			actions[pb.cursor]->Execute(pb.context);
			++pb.cursor;
		}
	}
	std::erase_if(activePlaybacks, [] (const Playback &pb) {
		return pb.list == nullptr || pb.cursor >= pb.list->actions.size();
		});
	return true;
}

const std::string_view &GameFeelEvents::NameClass() const {
	static constexpr std::string_view className = NAMEOF(Components::GameFeelEvents);
	return className;
}

bool GameFeelEvents::Read(Stream &stream) {
	if(!Component::Read(stream)) {
		return false;
	}
	allActionLists.clear();

	stream.ReadObject("Events", [this, &stream] (const std::string &eventName) {
		// ReadObject does NOT descend into each item, so push the event node first.
		if(!stream.PushNode(eventName)) {
			return;
		}
		stream.ReadArray("Actions", [this, &stream, &eventName] () {
			std::string type;
			if(!stream.Read("Type", type)) {
				LOG_WARNING("{}: action missing 'Type' in event '{}'", NameClass(), eventName);
				return;
			}
			auto action = GameFeelActionFactory::Create(type);
			if(action == nullptr) {
				LOG_WARNING("{}: unknown action type '{}'", NameClass(), type);
				return;
			}
			stream.Read("TriggerTime", action->triggerTime);  // Delay baked at parse time
			action->Read(stream);
			AddAction(eventName, action);
			});
		stream.PopNode();
		});

	// Cursor logic requires actions sorted by triggerTime.
	for(auto &[name, list] : allActionLists) {
		std::sort(list.actions.begin(), list.actions.end(),
			[] (const std::shared_ptr<IGameFeelAction> &a, const std::shared_ptr<IGameFeelAction> &b) {
				return a->triggerTime < b->triggerTime;
			});
	}
	return true;
}

void GameFeelEvents::AddAction(const std::string_view &name,
	const std::shared_ptr<IGameFeelAction> &action) {
	allActionLists[std::string{name}].actions.emplace_back(action);
}

bool GameFeelEvents::Play(const std::string_view &eventName) {
	const auto it = allActionLists.find(std::string{eventName});
	if(it == allActionLists.end()) {
		return false;
	}
	activePlaybacks.push_back(Playback{&it->second, 0.0f, 0, CaptureContext()});
	return true;
}

bool GameFeelEvents::PlayDetached(const std::string_view &eventName) {
	const auto it = allActionLists.find(std::string{eventName});
	if(it == allActionLists.end()) {
		return false;
	}
	auto *factory = RassGame::Systems::IGameFeelFactory::Get();
	if(factory == nullptr) {
		return false;
	}
	factory->PlayActions(it->second.actions, CaptureContext());   // capture BEFORE Destroy()
	return true;
}
bool GameFeelEvents::PlayDetachedAt(const std::string_view &eventName, const glm::vec3 &worldPos) {
	const auto it = allActionLists.find(std::string{eventName});
	if(it == allActionLists.end()) {
		return false;
	}
	auto *factory = RassGame::Systems::IGameFeelFactory::Get();
	if(factory == nullptr) {
		return false;
	}
	// Explicit position: don't capture from Parent() — the effect origin is the tile, not this entity.
	Systems::FeelContext ctx;
	ctx.position = worldPos;
	factory->PlayActions(it->second.actions, ctx);
	return true;
}
RassGame::Systems::FeelContext GameFeelEvents::CaptureContext() const {
	Systems::FeelContext ctx;
	if(Parent() != nullptr) {
		if(auto *t = Parent()->GetTransform()) {
			ctx.position = t->GetPosition();
			ctx.rotationRad = t->GetRotationRad();
		}
	}
	return ctx;
}

}
