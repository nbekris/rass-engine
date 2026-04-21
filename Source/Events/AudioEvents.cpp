// File Name:    AudioEvents.cpp
// Author(s):    main Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Event definitions for audio system events.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "AudioEvents.h"

#include "Events/GlobalEventID.h"
#include "Systems/GlobalEvents/IGlobalEventsSystem.h"
#include "Utils.h"

namespace RassEngine::Events {

const GlobalEventID AudioEvents::PlaySFXEvent{NAMEOF(AudioEvents::PlaySFXEvent)};
const GlobalEventID AudioEvents::PlaySFXRandomPitchEvent{NAMEOF(AudioEvents::PlaySFXRandomPitchEvent)};
const GlobalEventID AudioEvents::StopSFXEvent{NAMEOF(AudioEvents::StopSFXEvent)};
const GlobalEventID AudioEvents::PlayMusicEvent{NAMEOF(AudioEvents::PlayMusicEvent)};
const GlobalEventID AudioEvents::StopMusicEvent{NAMEOF(AudioEvents::StopMusicEvent)};
const GlobalEventID AudioEvents::LoadSoundEvent{NAMEOF(AudioEvents::LoadSoundEvent)};
const GlobalEventID AudioEvents::StreamMusicEvent{NAMEOF(AudioEvents::StreamMusicEvent)};

AudioEvents::Request AudioEvents::pendingRequest{};

void AudioEvents::PlaySFX(const std::string &soundName, float volume) {
	pendingRequest.soundName = soundName;
	pendingRequest.volume = volume;
	Systems::IGlobalEventsSystem::Get()->call(PlaySFXEvent);
}

void AudioEvents::PlaySFXRandomPitch(const std::string &soundName, float volume, float minPitch, float maxPitch) {
	pendingRequest.soundName = soundName;
	pendingRequest.volume = volume;
	pendingRequest.minPitch = minPitch;
	pendingRequest.maxPitch = maxPitch;
	Systems::IGlobalEventsSystem::Get()->call(PlaySFXRandomPitchEvent);
}

void AudioEvents::StopSFX(const std::string &soundName) {
	pendingRequest.soundName = soundName;
	Systems::IGlobalEventsSystem::Get()->call(StopSFXEvent);
}

void AudioEvents::PlayMusic(const std::string &soundName) {
	pendingRequest.soundName = soundName;
	Systems::IGlobalEventsSystem::Get()->call(PlayMusicEvent);
}

void AudioEvents::StopMusic() {
	Systems::IGlobalEventsSystem::Get()->call(StopMusicEvent);
}

void AudioEvents::LoadSound(const std::string &soundName) {
	pendingRequest.soundName = soundName;
	Systems::IGlobalEventsSystem::Get()->call(LoadSoundEvent);
}

void AudioEvents::StreamMusic(const std::string &soundName, bool isLooped) {
	pendingRequest.soundName = soundName;
	pendingRequest.isLooped = isLooped;
	Systems::IGlobalEventsSystem::Get()->call(StreamMusicEvent);
}

}
