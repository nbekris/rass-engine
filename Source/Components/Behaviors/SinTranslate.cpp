// File Name:    SinTranslate.cpp
// Author(s):    main Taro Omiya, secondary Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Behavior that translates an entity using a sine wave.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include <Precompiled.h>
#include "SinTranslate.h"

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

SinTranslate::SinTranslate() : Cloneable<Component, SinTranslate>{}
, onUpdateListener{this, &SinTranslate::Update} {}

SinTranslate::SinTranslate(const SinTranslate &other)
	: Cloneable<Component, SinTranslate>{other}
	, onUpdateListener{this, &SinTranslate::Update}
	, originalPosition{other.originalPosition}
	, magnitude{other.magnitude}
	, interval{other.interval}
	, offset{other.offset}
{}

SinTranslate::~SinTranslate() {
	if(Systems::IGlobalEventsSystem::Get() == nullptr) {
		return;
	}
	Systems::IGlobalEventsSystem::Get()->unbind(Events::Global::Update, &onUpdateListener);
}

bool SinTranslate::Initialize() {
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
	originalPosition = Parent()->GetTransform()->GetLocalPosition();
	return true;
}

const std::string_view &SinTranslate::NameClass() const {
	static constexpr std::string_view className = NAMEOF(Components::Behaviors::SinTranslate);
	return className;
}

bool SinTranslate::Read(Stream &stream) {
	if(!Component::Read(stream)) {
		return false;
	}
	// Read the magnitude and interval from the stream
	stream.ReadVec3("Magnitude", magnitude);
	stream.ReadVec3("Interval", interval);
	stream.ReadVec3("Offset", offset);
	return true;
}

bool SinTranslate::Update(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &) {
	// Calculate how much to adjust the position
	float timePassed = Systems::ITimeSystem::Get()->GetTimePassedSec();
	glm::vec3 newPosition = originalPosition;
	newPosition.x = (timePassed * interval.x + offset.x);
	newPosition.x = std::sinf(newPosition.x) * magnitude.x + 1.0f;
	newPosition.x *= originalPosition.x;

	newPosition.y = (timePassed * interval.y + offset.y);
	newPosition.y = std::sinf(newPosition.y) * magnitude.y + 1.0f;
	newPosition.y *= originalPosition.y;

	newPosition.z = (timePassed * interval.z + offset.z);
	newPosition.z = std::sinf(newPosition.z) * magnitude.z + 1.0f;
	newPosition.z *= originalPosition.z;

	// Set the new scale
	Transform *transform = Parent()->GetTransform();
	transform->SetLocalPosition(newPosition);
	return true;
}

}
