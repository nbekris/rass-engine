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
#include "TweenCurve.h"

using namespace RassEngine;
using namespace RassEngine::Events;
using namespace RassEngine::Components;
using namespace RassEngine::Systems;

namespace RassEngine::Components {

static constexpr std::string_view KEY_FADE_DURATION = "FadeDuration";
static constexpr std::string_view KEY_AUTO_DISABLE = "AutoDisable";

SpriteFader::SpriteFader() : Cloneable<Component, SpriteFader>{},
onUpdateListener{this, &SpriteFader::OnUpdate}  {
	UpdateLinearCurve();
}

SpriteFader::SpriteFader(const SpriteFader &other)
	: Cloneable<Component, SpriteFader>{other}, fadeDuration{other.fadeDuration}
	, curve{other.curve}, autoDisable{other.autoDisable}
	, onUpdateListener{this, &SpriteFader::OnUpdate}
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

	// Read whether to auto-disable
	stream.Read(KEY_AUTO_DISABLE, autoDisable);

	// Read the fade duration first
	if(stream.Read(KEY_FADE_DURATION, fadeDuration)) {
		// If available, setup a linear curve
		UpdateLinearCurve();
		return true;
	}

	// Otherwise, read the curve values
	curve.Read(stream);
	fadeDuration = curve.GetTotalDuration();
	return true;
}

bool SpriteFader::OnUpdate(const IEvent<GlobalEventArgs> *, const GlobalEventArgs &) {
	if(!IsEnabled()) {
		// If not enabled, don't update this sprite
		return true;
	}

	// Compute the current time
	float dt = ITimeSystem::Get()->GetDeltaTimeSec();
	currentTime += dt;

	// Update the alpha to the curve's value
	if(GetSprite()) {
		GetSprite()->Alpha(curve.Calculate(currentTime));
	}

	// Auto-disable this component to save processing
	if(autoDisable && (currentTime > fadeDuration)) {
		SetEnabled(false);
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
