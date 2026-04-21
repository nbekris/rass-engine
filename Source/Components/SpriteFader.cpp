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
#include <Entity.h>
#include <Cloneable.h>
#include <Component.h>
#include <Components/Sprite.h>
#include <Events/Global.h>
#include <Scenes/SerializedScene.h>
#include <Systems/Time/ITimeSystem.h>
#include <Systems/Scene/ISceneSystem.h>
#include <Systems/GlobalEvents/IGlobalEventsSystem.h>

using namespace RassEngine;
using namespace RassEngine::Events;
using namespace RassEngine::Components;
using namespace RassEngine::Scenes;
using namespace RassEngine::Systems;

namespace RassEngine::Components {

static constexpr std::string_view KEY_FADE_DURATION = "FadeDuration";

SpriteFader::SpriteFader() : Cloneable<Component, SpriteFader>{},
onUpdateListener{this, &SpriteFader::OnUpdate}  {}

	SpriteFader::SpriteFader(const SpriteFader &other)
		: Cloneable<Component, SpriteFader>{}, fadeDuration{other.fadeDuration},
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

		Sprite *spr = Parent()->Get<Sprite>();
		if(spr) {
			spr->Alpha(0.0f);
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

		stream.Read(KEY_FADE_DURATION, fadeDuration);

		return true;
	}

	bool SpriteFader::OnUpdate(const IEvent<GlobalEventArgs> *, const GlobalEventArgs &) {
		float dt = ITimeSystem::Get()->GetDeltaTimeSec();
		currentTime += dt;

		Sprite *spr = Parent()->Get<Sprite>();
		if(spr) {
			float alpha = std::clamp(currentTime / fadeDuration, 0.0f, 1.0f);
			spr->Alpha(alpha);
		}

		return true;
	}
}
