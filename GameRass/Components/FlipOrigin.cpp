// File Name:    FlipOrigin.cpp
// Author(s):    main Taro Omiya, secondary Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Component defining the origin point for flip mechanics.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "pch.h"
#include "FlipOrigin.h"

#include <Cloneable.h>
#include <Component.h>
#include <Components/Transform.h>
#include <Entity.h>
#include <Events/EventSynchronous.h>
#include <Events/Global.h>
#include <Events/GlobalEventArgs.h>
#include <IEvent.h>
#include <memory>
#include <Stream.h>
#include <string_view>
#include <Systems/GlobalEvents/IGlobalEventsSystem.h>
#include <Systems/Logging/ILoggingSystem.h>
#include <Systems/Scene/ISceneSystem.h>
#include <Systems/Time/ITimeSystem.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace RassEngine;
using namespace RassEngine::Components;
using namespace RassEngine::Events;
using namespace RassEngine::Systems;

namespace RassGame::Components {

FlipOrigin::FlipOrigin() : Cloneable<Component, FlipOrigin>{}
, onUpdateListener{this, &FlipOrigin::Update}
, onFlipStart{}, onFlipEnd{}
, entityToFollowName{}, transformToFollow{nullptr}
, isAnimating{false}, isRightSideUp{true}
{
	onFlipStart = std::make_unique<EventSynchronous<FlipEventArgs>>();
	onFlipEnd = std::make_unique<EventSynchronous<FlipEventArgs>>();
}

FlipOrigin::FlipOrigin(const FlipOrigin &other)
	: Cloneable<Component, FlipOrigin>{other}
	, onUpdateListener{this, &FlipOrigin::Update}
	, onFlipStart{}, onFlipEnd{}
	, entityToFollowName{other.entityToFollowName}
	, transformToFollow{other.transformToFollow}
	, isAnimating{false}
	, isRightSideUp{other.isRightSideUp}
{
	onFlipStart = std::make_unique<EventSynchronous<FlipEventArgs>>();
	onFlipEnd = std::make_unique<EventSynchronous<FlipEventArgs>>();
}

FlipOrigin::~FlipOrigin() {
	if(IGlobalEventsSystem::Get() == nullptr) {
		return;
	}
	IGlobalEventsSystem::Get()->unbind(Global::Update, &onUpdateListener);
}

bool FlipOrigin::Initialize() {
	if(IGlobalEventsSystem::Get() == nullptr) {
		return false;
	}

	// Bind to update function
	IGlobalEventsSystem::Get()->bind(Global::Update, &onUpdateListener);

	// Grab the entity to follow
	const Entity *entityToFollow = ISceneSystem::Get()->FindEntity(entityToFollowName);
	if(entityToFollow) {
		// Grab the transform to follow
		transformToFollow = entityToFollow->GetTransform();

		// Set this transform to follow this
		Transform *thisTransform = Parent()->GetTransform();
		thisTransform->SetParentTransform(transformToFollow);
		thisTransform->SetLocalPosition(glm::vec3{0.0f, 0.0f, 0.0f});
	}

	// Check if we're following anything
	if (transformToFollow == nullptr) {
		LOG_WARNING("{}: Could not find entity to follow with name '{}'", NameClass(), entityToFollowName);
	}
	return true;
}

const std::string_view &FlipOrigin::NameClass() const {
	static constexpr std::string_view className = NAMEOF(Components::FlipOrigin);
	return className;
}

bool FlipOrigin::Read(Stream &stream) {
	if(!Component::Read(stream)) {
		return false;
	}
	stream.Read("EntityToFollow", entityToFollowName);
	return true;
}

bool FlipOrigin::StartFlip() {
	// Don't flip if we're already in the middle of it
	if(isAnimating) {
		return false;
	}

	// Stop time
	isAnimating = true;
	ITimeSystem::Get()->SetTimeScale(0.0f);

	// Unfollow the target, and set the scale unflipped
	Transform *transform = Parent()->GetTransform();
	transform->SetParentTransform(nullptr);
	transform->SetLocalScale(glm::vec3{1.0f, 1.0f, 1.0f});

	// Toggle flip flag
	isRightSideUp = !isRightSideUp;

	// Call flip start event
	return onFlipStart->call(FlipEventArgs(transform, isRightSideUp));
}

bool FlipOrigin::Update(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &) {
	// Check if we're animating
	if(!isAnimating) {
		return true;
	}

	// Grab the transform
	Transform *transform = Parent()->GetTransform();

	// Perform actual animation here
	float currentYScale = transform->GetLocalScale().y;
	if(currentYScale > -1.0f) {
		currentYScale -= (2.0f / ANIMATION_DURATION_SECONDS) * ITimeSystem::Get()->GetUnscaledDeltaTime();
		transform->SetLocalScale(glm::vec3{1.0f, currentYScale, 1.0f});
		return true;
	}

	// Snap to the flipped orientation
	isAnimating = false;
	transform->SetLocalScale(glm::vec3{1.0f, -1.0f, 1.0f});

	// Resume time
	ITimeSystem::Get()->SetTimeScale(1.0f);

	// Call flip end event
	bool toReturn = onFlipEnd->call(FlipEventArgs(transform, isRightSideUp));

	// Follow the target again
	transform->SetParentTransform(transformToFollow);
	transform->SetLocalPosition(glm::vec3{0.0f, 0.0f, 0.0f});
	return toReturn;
}

}
