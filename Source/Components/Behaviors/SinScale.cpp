// File Name:    SinScale.cpp
// Author(s):    main Taro Omiya, secondary Niko Bekris, Eric Fleegal
// Course:       GAM541
// Project:      RASS
// Purpose:      Behavior that scales an entity using a sine wave.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "SinScale.h"

#include <cmath>

#include "Cloneable.h"
#include "Component.h"
#include "Components/Transform.h"
#include "Entity.h"
#include "Events/Global.h"
#include "Events/GlobalEventArgs.h"
#include "IEvent.h"
#include "Stream.h"
#include "Systems/GlobalEvents/IGlobalEventsSystem.h"
#include "Systems/Logging/ILoggingSystem.h"
#include "Systems/Time/ITimeSystem.h"

namespace RassEngine::Components::Behaviors {

SinScale::SinScale() : Cloneable<Component, SinScale>{}
, onUpdateListener{this, &SinScale::Update} {}

SinScale::SinScale(const SinScale &other)
	: Cloneable<Component, SinScale>{other}
	, onUpdateListener{this, &SinScale::Update}
	, originalScale{other.originalScale}
	, magnitude{other.magnitude}
	, interval{other.interval}
	, offset{other.offset}
{}

SinScale::~SinScale() {
	if(Systems::IGlobalEventsSystem::Get() == nullptr) {
		return;
	}
	Systems::IGlobalEventsSystem::Get()->unbind(Events::Global::Update, &onUpdateListener);
}

bool SinScale::Initialize() {
	if(Systems::IGlobalEventsSystem::Get() == nullptr) {
		LOG_ERROR("Cannot bind to {}", NAMEOF(Systems::IGlobalEventsSystem));
		return false;
	} else if(Systems::ITimeSystem::Get() == nullptr) {
		LOG_ERROR("Cannot retrieve {}", NAMEOF(Systems::ITimeSystem));
		return false;
	} else if(Parent() == nullptr) {
		LOG_ERROR("Unable to retrieve parent");
		return false;
	} else if(Parent()->GetTransform() == nullptr) {
		LOG_ERROR("Unable to retrieve transform");
		return false;
	}

	Systems::IGlobalEventsSystem::Get()->bind(Events::Global::Update, &onUpdateListener);

	// Setup scale
	originalScale = Parent()->GetTransform()->GetLocalScale();
	return true;
}

const std::string_view &SinScale::NameClass() const {
	static constexpr std::string_view className = NAMEOF(Components::Behaviors::SinScale);
	return className;
}

bool SinScale::Read(Stream &stream) {
	if(!Component::Read(stream)) {
		return false;
	}
	// Read the magnitude and interval from the stream
	stream.Read("Magnitude", magnitude);
	stream.Read("Interval", interval);
	stream.Read("Offset", offset);
	return true;
}

bool SinScale::Update(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &) {
	// Calculate how much to adjust the scale
	float timePassed = Systems::ITimeSystem::Get()->GetTimePassedSec();
	float newScale = (timePassed * interval + offset);
	newScale = std::sinf(newScale) * magnitude + 1.0f;

	// Set the new scale
	Transform* transform = Parent()->GetTransform();
	transform->SetLocalScale(originalScale * newScale);
	return true;
}

}
