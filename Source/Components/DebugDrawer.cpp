// File Name:    DebugDrawer.cpp
// Author(s):    main Boyuan Hu, secondary Niko Bekris, Taro Omiya, Eric Fleegal
// Course:       GAM541
// Project:      RASS
// Purpose:      Component for rendering debug visualization shapes.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "Components/DebugDrawer.h"

#include "Component.h"
#include "Components/Sprite.h"
#include "Entity.h"
#include "Events/Global.h"
#include "Events/GlobalEventArgs.h"
#include "IEvent.h"
#include "Cloneable.h"
#include "Systems/GlobalEvents/IGlobalEventsSystem.h"
#include "Systems/Time/ITimeSystem.h"
#include "Systems/Logging/ILoggingSystem.h"

namespace RassEngine::Components {
DebugDrawer::DebugDrawer()
	: Cloneable<Component, DebugDrawer>{}
	, onUpdateListener{this, &DebugDrawer::Update} {}

DebugDrawer::DebugDrawer(const DebugDrawer &other)
	: Cloneable<Component, DebugDrawer>{other}
	, onUpdateListener{this, &DebugDrawer::Update} {
}

DebugDrawer::~DebugDrawer() {
	if(Systems::IGlobalEventsSystem::Get() == nullptr) {
		return;
	}
	Systems::IGlobalEventsSystem::Get()->unbind(Events::Global::Update, &onUpdateListener);
}

bool DebugDrawer::Initialize() {
	if(Systems::IGlobalEventsSystem::Get() == nullptr) {
		LOG_ERROR("Cannot initialize DebugDrawer: {} is not registered", NAMEOF(Systems::IGlobalEventsSystem));
		return false;
	}

	Systems::IGlobalEventsSystem::Get()->bind(Events::Global::Update, &onUpdateListener);
	return true;
}

bool DebugDrawer::Read(Stream &stream) {
	if(!Component::Read(stream)) {
		return false;
	}
	stream.Read("ShowFPS", showFPS);

	return true;
}

const std::string_view &DebugDrawer::NameClass() const {
	static constexpr std::string_view className = NAMEOF(RassEngine::Components::DebugDrawer);
	return className;
}

bool DebugDrawer::Update(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &args) {
	if(!sprite) {
		sprite = Parent()->Get<Sprite>();
	}
	if(showFPS)
	{
		if(sprite) {
			sprite->SetText("FPS: " + std::to_string(static_cast<int>(Systems::ITimeSystem::Get()->GetCurrentFPS())));
		}
	}

	return true;
}
}

