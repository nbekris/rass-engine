// File Name:    IAudioSystem.h
// Author(s):    main Niko Bekris, secondary Boyuan Hu
// Course:       GAM541
// Project:      RASS
// Purpose:      System managing audio playback and mixing.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <string_view>

#include "Systems/GlobalEvents/IGlobalSystem.h"

namespace RassEngine::Systems {
	class IAudioSystem : public IGlobalSystem<IAudioSystem> {
	public:
		virtual ~IAudioSystem() = default;
		virtual bool Initialize() = 0;
		virtual void Shutdown() = 0;
		virtual const std::string_view &NameClass() const = 0;
		virtual void StreamMusic(const std::string_view &filename, bool isLooped) = 0;
		virtual void PlayMusic(const std::string_view &filePath) = 0;
		virtual void StopMusic() = 0;
		virtual void LoadSound(const std::string_view &filename) = 0;
		virtual void PlaySFX(const std::string_view &soundName, float volume = 1.0f) = 0;
		virtual void PlaySFXRandomPitch(const std::string_view &soundName, float volume = 1.0f, float minPitch = 0.8f, float maxPitch = 1.2f) = 0;
		virtual void StopSFX(const std::string_view &soundName) = 0;


		virtual void SetMusicMuted(bool muted) = 0;
		virtual bool IsMusicMuted() const = 0;

		virtual void SetMusicVolume(float volume) = 0;
		virtual float GetMusicVolume() const = 0;
		virtual void SetSFXVolume(float volume) = 0;
		virtual float GetSFXVolume() const = 0;

	protected:
		IAudioSystem() = default;
	};
}
