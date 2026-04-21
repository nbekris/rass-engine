// File Name:    SinRotate.cpp
// Author(s):    main Taro Omiya, secondary Niko Bekris, Eric Fleegal
// Course:       GAM541
// Project:      RASS
// Purpose:      Behavior that rotates an entity using a sine wave.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "SinRotate.h"

#include <cmath>
#include <string_view>

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

SinRotate::SinRotate() : Cloneable<Component, SinRotate>{}
, onUpdateListener{this, &SinRotate::Update} {}

SinRotate::SinRotate(const SinRotate &other)
	: Cloneable<Component, SinRotate>{other}
	, onUpdateListener{this, &SinRotate::Update}
	, originalRotation{other.originalRotation}
	, magnitude{other.magnitude}
	, interval{other.interval}
	, offset{other.offset}
{}

SinRotate::~SinRotate() {
	if(Systems::IGlobalEventsSystem::Get() == nullptr) {
		return;
	}
	Systems::IGlobalEventsSystem::Get()->unbind(Events::Global::Update, &onUpdateListener);
}

bool SinRotate::Initialize() {
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

	// Setup rotation
	originalRotation = Parent()->GetTransform()->GetLocalRotationRad();
	return true;
}

const std::string_view &SinRotate::NameClass() const {
	static constexpr std::string_view className = NAMEOF(Components::Behaviors::SinRotate);
	return className;
}

bool SinRotate::Read(Stream &stream) {
	if(!Component::Read(stream)) {
		return false;
	}
	// Read the magnitude and interval from the stream
	stream.Read("Magnitude", magnitude);
	stream.Read("Interval", interval);
	stream.Read("Offset", offset);
	return true;
}

bool SinRotate::Update(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &) {
	// Calculate how much to adjust the rotation
	float timePassed = Systems::ITimeSystem::Get()->GetTimePassedSec();
	float addRotation = (timePassed * interval + offset);
	addRotation = std::sinf(addRotation) * magnitude;

	// Set the new scale
	Transform* transform = Parent()->GetTransform();
	transform->SetLocalRotationRad(originalRotation + addRotation);
	return true;
}

}
