// File Name:    Flippable.cpp
// Author(s):    main Taro Omiya, secondary Niko Bekris, Boyuan Hu
// Course:       GAM541
// Project:      RASS
// Purpose:      Component marking an entity as able to be flipped.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "pch.h"
#include "Flippable.h"

#include <Cloneable.h>
#include <Component.h>
#include <Components/Transform.h>
#include <Components/PhysicsBody.h>
#include <Entity.h>
#include <IEvent.h>
#include <Stream.h>
#include <string_view>
#include <Systems/Logging/ILoggingSystem.h>
#include <Systems/Scene/ISceneSystem.h>
#include <glm/vec3.hpp>

using namespace RassEngine;
using namespace RassEngine::Components;
using namespace RassEngine::Events;
using namespace RassEngine::Systems;

namespace RassGame::Components {

const float GROUNDHEIGHT = 0.0f;

Flippable::Flippable() : Cloneable<Component, Flippable>{}
, onFlipStartListener{this, &Flippable::StartFlip}
, onFlipEndListener{this, &Flippable::EndFlip}
, animateScaling{true}, lastTransform{nullptr}, lastLocalScale{1.f, 1.f, 1.f}
{}

Flippable::Flippable(const Flippable &other)
	: Cloneable<Component, Flippable>{other}
	, onFlipStartListener{this, &Flippable::StartFlip}
	, onFlipEndListener{this, &Flippable::EndFlip}
	, animateScaling{other.animateScaling}, lastTransform{nullptr}, lastLocalScale{1.f, 1.f, 1.f}
{}

Flippable::~Flippable() {
	if(ISceneSystem::Get() == nullptr) {
		return;
	}

	Entity *unbindEntity = ISceneSystem::Get()->FindEntity(FLIP_ORIGIN_NAME);
	if(unbindEntity == nullptr) {
		return;
	}

	FlipOrigin *unbindComponent = unbindEntity->Get<FlipOrigin>();
	if(unbindComponent == nullptr) {
		return;
	}

	// Unbind from events
	unbindComponent->UnbindOnFlipStart(&onFlipStartListener);
	unbindComponent->UnbindOnFlipEnd(&onFlipEndListener);
}

bool Flippable::Initialize() {
	Transform *transform = Parent()->GetTransform();
	if(transform == nullptr) {
		LOG_WARNING("{}: {} requires '{}' component", NameClass(), NameClass(), NAMEOF(Transform));
		return false;
	}

	if(ISceneSystem::Get() == nullptr) {
		LOG_WARNING("{}: System '{}' is not registered", NameClass(), NAMEOF(ISceneSystem));
		return false;
	}

	Entity *bindEntity = ISceneSystem::Get()->FindEntity(FLIP_ORIGIN_NAME);
	if(bindEntity == nullptr) {
		LOG_WARNING("{}: Entity '{}' was not found in scene", NameClass(), FLIP_ORIGIN_NAME);
		return false;
	}

	FlipOrigin *bindComponent = bindEntity->Get<FlipOrigin>();
	if(bindComponent == nullptr) {
		LOG_WARNING("{}: Component '{}' was not found in entity '{}'", NameClass(), NAMEOF(FlipOrigin), FLIP_ORIGIN_NAME);
		return false;
	}

	// Bind to events
	bindComponent->BindOnFlipStart(&onFlipStartListener);
	bindComponent->BindOnFlipEnd(&onFlipEndListener);
	return true;
}

const std::string_view &Flippable::NameClass() const {
	static constexpr std::string_view className = NAMEOF(Components::Flippable);
	return className;
}

bool Flippable::Read(Stream &stream) {
	if(!Component::Read(stream)) {
		return false;
	}
	stream.Read("AnimateScaling", animateScaling);
	return true;
}

bool Flippable::StartFlip(const IEvent<FlipOrigin::FlipEventArgs> *, const FlipOrigin::FlipEventArgs &args) {
	// FIXME: do something different if animateScaling is false
	// For now, we're just ignoring the flag and always animating

	// Check if there's a transform on flip event args
	if(args.animatedTransform == nullptr) {
		LOG_WARNING("{}: {} does not contain a transform", NameClass(), NAMEOF(FlipOrigin::FlipEventArgs));
		return false;
	}

	// Cache the last transform and scale
	Transform *transform = Parent()->GetTransform();
	lastTransform = transform->GetParentTransform();
	lastLocalScale = transform->GetLocalScale();

	// Change parent to this transform
	transform->SetParentTransform(args.animatedTransform);
	LOG_INFO("{}: Flip Start Scale Y local '{}' global '{}'", NameClass(), transform->GetLocalScale().y, transform->GetLossyScale().y);
	return true;
}

bool Flippable::EndFlip(const IEvent<FlipOrigin::FlipEventArgs> *, const FlipOrigin::FlipEventArgs &args) {
	// Revert the parent and scale
	Transform *transform = Parent()->Get<Transform>();
	transform->SetParentTransform(lastTransform);

	// Snap the scale
	if(animateScaling) {
		lastLocalScale.y *= -1.0f;
	}
	transform->SetLocalScale(lastLocalScale);
	LOG_INFO("{}: Flip End Scale Y local '{}' global '{}'", NameClass(), transform->GetLocalScale().y, transform->GetLossyScale().y);
	return true;
}

// FIXME: when implementing animateScaling, consider binding to the global update function
//bool Flippable::Update(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &) {
//	return true;
//}

}
