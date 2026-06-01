// File Name:    SpriteFader.cpp
// Author(s):    main Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Component that fades a sprite alpha in or out.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "SpriteFader.h"

#include "Stream.h"
#include "Utils.h"
#include "Cloneable.h"
#include "Component.h"
#include "Components/Sprite.h"
#include "Entity.h"
#include "Events/Global.h"
#include "Systems/GlobalEvents/IGlobalEventsSystem.h"
#include "Systems/Time/ITimeSystem.h"

using namespace RassEngine;
using namespace RassEngine::Events;
using namespace RassEngine::Components;
using namespace RassEngine::Systems;

namespace RassEngine::Components {

static constexpr std::string_view KEY_FADE_DURATION = "FadeDuration";

SpriteFader::SpriteFader() : Cloneable<Component, SpriteFader>{},
onUpdateListener{this, &SpriteFader::OnUpdate}  {}

SpriteFader::SpriteFader(const SpriteFader &other)
	: Cloneable<Component, SpriteFader>{other}, fadeDuration{other.fadeDuration},
	onUpdateListener{this, &SpriteFader::OnUpdate}
{
}

SpriteFader::~SpriteFader() {
	if(IGlobalEventsSystem::Get() != nullptr) {
		IGlobalEventsSystem::Get()->unbind(Events::Global::Update, &onUpdateListener);
	}
}

bool SpriteFader::Initialize() {
	if(IGlobalEventsSystem::Get() != nullptr) {
		IGlobalEventsSystem::Get()->bind(Events::Global::Update, &onUpdateListener);
	}

	if(GetSprite()) {
		GetSprite()->Alpha(0.0f);
	}

	return true;
}

const std::string_view &SpriteFader::NameClass() const {
	static constexpr std::string_view className = NAMEOF(SpriteFader);
	return className;
}

bool SpriteFader::Read(Stream &stream) {
	if(!Component::Read(stream)) {
		return false;
	}

	// FIXME: read a tween curve, instead
	stream.Read(KEY_FADE_DURATION, fadeDuration);

	return true;
}

bool SpriteFader::OnUpdate(const IEvent<GlobalEventArgs> *, const GlobalEventArgs &) {
	// If not enabled, don't update this sprite
	if(!IsEnabled()) {
		return true;
	}

	float dt = ITimeSystem::Get()->GetDeltaTimeSec();
	currentTime += dt;

	if(GetSprite()) {
		float alpha = std::clamp(currentTime / fadeDuration, 0.0f, 1.0f);
		GetSprite()->Alpha(alpha);
	}

	return true;
}

Sprite *SpriteFader::GetSprite() const {
	if(spriteCache == nullptr) {
		spriteCache = Parent()->Get<Sprite>();
	}
	return spriteCache;
}
}
