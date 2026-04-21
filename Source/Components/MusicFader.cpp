// File Name:    MusicFader.cpp
// Author(s):    main Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Component that fades music volume in or out.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "MusicFader.h"

#include "Stream.h"
#include "Utils.h"
#include "Systems/Audio/IAudioSystem.h"

namespace RassEngine::Components {
	MusicFader::MusicFader() : Cloneable<Component, MusicFader>{} {}

	MusicFader::MusicFader(const MusicFader &other)
		: Cloneable<Component, MusicFader>{}
		, fileName{other.fileName}
		, volume{other.volume}
		, pitch{other.pitch}
		, isLooping{other.isLooping}
		, playOnInit{other.playOnInit}
	{
	}

	MusicFader::~MusicFader() {
	}

	bool MusicFader::Initialize() {
		if(!Systems::IAudioSystem::Get()) {
			LOG_WARNING("{}: No audio system found, cannot play audio source: {}", NameClass(), fileName);
			return false;
		}

		// Audio has to be loaded first before it becomes playable
		Systems::IAudioSystem::Get()->StreamMusic(fileName, isLooping);
		if(playOnInit) {
			Play();
		}
		return true;
	}

	const std::string_view &MusicFader::NameClass() const {
		static constexpr std::string_view className = NAMEOF(MusicFader);
		return className;
	}

	bool MusicFader::Read(Stream &stream) {
		if(!Component::Read(stream)) {
			return false;
		}

		stream.Read("FileName", fileName);
		stream.Read("Volume", volume);
		stream.Read("Pitch", pitch);
		stream.Read("IsLooping", isLooping);
		stream.Read("PlayOnInit", playOnInit);
		return true;
	}

	void MusicFader::Play() const {
		if(!Systems::IAudioSystem::Get()) {
			LOG_WARNING("{}: {} is not registered, cannot play audio source: {}", NameClass(), NAMEOF(Systems::IAudioSystem), fileName);
			return;
		}
		Systems::IAudioSystem::Get()->PlayMusic(fileName);
	}

	void MusicFader::Stop() const {
		if(!Systems::IAudioSystem::Get()) {
			LOG_WARNING("{}: {} is not registered, cannot stop audio source: {}", NameClass(), NAMEOF(Systems::IAudioSystem), fileName);
			return;
		}
		Systems::IAudioSystem::Get()->StopMusic();
	}

}
