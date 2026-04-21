// File Name:    Animation.cpp
// Author(s):    main Boyuan Hu, secondary Taro Omiya, Steven Yacoub, Niko Bekris, Eric Fleegal
// Course:       GAM541
// Project:      RASS
// Purpose:      Component handling sprite animation playback.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "Components/Animation.h"

#include "Component.h"
#include "Components/Sprite.h"
#include "Entity.h"
#include "Events/Global.h"
#include "Events/GlobalEventArgs.h"
#include "IEvent.h"
#include "Stream.h"
#include "Systems/GlobalEvents/IGlobalEventsSystem.h"
#include "Systems/Logging/ILoggingSystem.h"
#include "Systems/Time/ITimeSystem.h"

namespace RassEngine::Components {

Animation::Animation()
	: Cloneable<Component, Animation>{}
	, onUpdateListener{this, &Animation::Update} {}

Animation::Animation(const Animation &other)
	: Cloneable<Component, Animation>{other}
	, onUpdateListener{this, &Animation::Update}
	, frameIndex{other.frameIndex}
	, frameCount{other.frameCount}
	, frameDuration{other.frameDuration}
	, isRunning{other.isRunning}
	, isLooping{other.isLooping}
	, isDone{other.isDone} {}

Animation::~Animation() {
	if(Systems::IGlobalEventsSystem::Get() == nullptr) {
		return;
	}
	Systems::IGlobalEventsSystem::Get()->unbind(Events::Global::Update, &onUpdateListener);
}

bool Animation::Initialize() {
	// Make sure all systems are available
	if(Systems::IGlobalEventsSystem::Get() == nullptr) {
		LOG_ERROR("Cannot animate: {} is not registered", NAMEOF(Systems::IGlobalEventsSystem));
		return false;
	} else if(Systems::ITimeSystem::Get() == nullptr) {
		LOG_ERROR("Cannot animate: {} is not registered", NAMEOF(Systems::ITimeSystem));
		return false;
	}

	// Bind to the update events
	Systems::IGlobalEventsSystem::Get()->bind(Events::Global::Update, &onUpdateListener);
	return true;
}

void Animation::AdvanceFrame() {
	Sprite *sprite = Parent()->Get<Sprite>();

	if(sprite) {
		frameIndex++;
		if(frameIndex >= frameCount) {
			if(isLooping) {
				frameIndex = 0;
				isDone = true;
			} else {
				frameCount = frameCount - 1;
				isRunning = false;
				isDone = true;
			}
		}
		if(isRunning) {
			sprite->SetFrame(frameIndex);
			//sprite->Update(frameDuration);
			frameDelay = frameDuration;
		} else {
			frameDelay = 0.0f;
		}
		frameDelay = frameDuration;
	}
}

bool Animation::Read(Stream& stream) {
	if(!Component::Read(stream)) {
		return false;
	}

	stream.Read("FrameCount", frameCount);
	stream.Read("FrameDuration", frameDuration);
	stream.Read("IsLooping", isLooping);
	stream.Read("IsRunning", isRunning);
	return true;
}

const std::string_view &Animation::NameClass() const {
	static constexpr std::string_view className = NAMEOF(RassEngine::Components::Animation);
	return className;
}


bool Animation::Update(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &args) {
	// Make sure the time system is available
	if(Systems::ITimeSystem::Get() == nullptr) {
		LOG_ERROR("Cannot animate: {} is not registered", NAMEOF(Systems::ITimeSystem));
		return false;
	}

	// Retrieve the delta time from timesystem
	float dt = Systems::ITimeSystem::Get()->GetDeltaTimeSec();

	if(Parent()->Get<Sprite>()) {
		isDone = false;
		if(isRunning) {
			frameDelay -= dt;
			if(frameDelay <= 0.0f) {
				AdvanceFrame();
			}
		}
	}

	return true;
}
}
