// File Name:    AudioSource.cpp
// Author(s):    main Niko Bekris, secondary Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Component for playing audio at an entity location.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "AudioSource.h"

#include "Stream.h"
#include "Utils.h"
#include "Systems/Audio/IAudioSystem.h"

namespace RassEngine::Components {
	AudioSource::AudioSource() : Cloneable<Component, AudioSource>{} {}

	AudioSource::AudioSource(const AudioSource &other)
		: Cloneable<Component, AudioSource>{}
		, fileName{other.fileName}
		, volume{other.volume}
		, pitch{other.pitch}
		, isLooping{other.isLooping}
		, isSpatial{other.isSpatial}
	{
	}

	AudioSource::~AudioSource() {
	}

	bool AudioSource::Initialize() {
		if(!Systems::IAudioSystem::Get()) {
			LOG_WARNING("{}: No audio system found, cannot play audio source: {}", NameClass(), fileName);
			return false;
		}

		// Audio has to be loaded first before it becomes playable
		Systems::IAudioSystem::Get()->LoadSound(fileName);
		return true;
	}

	const std::string_view &AudioSource::NameClass() const {
		static constexpr std::string_view className = NAMEOF(AudioSource);
		return className;
	}

	bool AudioSource::Read(Stream &stream) {
		if(!Component::Read(stream)) {
			return false;
		}

		stream.Read("FileName", fileName);
		stream.Read("Volume", volume);
		stream.Read("Pitch", pitch);
		stream.Read("IsLooping", isLooping);
		stream.Read("IsSpatial", isSpatial);
		return true;
	}

	void AudioSource::Play() const {
		if(!Systems::IAudioSystem::Get()) {
			LOG_WARNING("{}: {} is not registered, cannot play audio source: {}", NameClass(), NAMEOF(Systems::IAudioSystem), fileName);
			return;
		}
		Systems::IAudioSystem::Get()->PlaySFX(fileName, volume);
	}

	void AudioSource::Stop() const {
		if(!Systems::IAudioSystem::Get()) {
			LOG_WARNING("{}: {} is not registered, cannot stop audio source: {}", NameClass(), NAMEOF(Systems::IAudioSystem), fileName);
			return;
		}
		Systems::IAudioSystem::Get()->StopSFX(fileName);
	}

}
