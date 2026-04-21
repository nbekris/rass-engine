// File Name:    AudioSystem.h
// Author(s):    main Niko Bekris, secondary Boyuan Hu
// Course:       GAM541
// Project:      RASS
// Purpose:      System managing audio playback and mixing.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <fmod.hpp>
#include "IAudioSystem.h"
#include "Events/GlobalEventArgs.h"
#include "Events/GlobalEventListener.h"

namespace RassEngine::Systems {
	class AudioSystem : public IAudioSystem {
		public:
			AudioSystem();
			~AudioSystem();
			bool Initialize();
			void Shutdown() override;
			const std::string_view &NameClass() const override;
			bool Update(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &);

			void LoadSound(const std::string_view &filename) override;
			void StreamMusic(const std::string_view &filename, bool isLooped) override;
			void PlaySFX(const std::string_view &soundName, float volume = 1.0f) override;
			void PlaySFXRandomPitch(const std::string_view &soundName, float volume = 1.0f, float minPitch = 0.8f, float maxPitch = 1.2f) override;
			void PlayMusic(const std::string_view &filePath) override;
			void StopMusic() override;
			void StopSFX(const std::string_view &soundName) override;
			void Crossfade(std::string name, float duration = 2.0f);

			// Mute music channel group only (SFX unaffected)
			void SetMusicMuted(bool muted) override;
			bool IsMusicMuted() const override;

			// Volume control for music and SFX channel groups
			void SetMusicVolume(float volume) override;
			float GetMusicVolume() const override;
			void SetSFXVolume(float volume) override;
			float GetSFXVolume() const override;

			// Audio event handlers
			bool OnPlaySFX(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &);
			bool OnPlaySFXRandomPitch(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &);
			bool OnStopSFX(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &);
			bool OnPlayMusic(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &);
			bool OnStopMusic(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &);
			bool OnLoadSound(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &);
			bool OnStreamMusic(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &);

	private:
		bool IsChannelPlaying(FMOD::Channel *channel);
		void ShutDownAudio();
		void CheckResult(FMOD_RESULT result);
		void CrossfadeUpdate(float dt);

	private:
			Events::GlobalEventListener<AudioSystem> updateListener;
			Events::GlobalEventListener<AudioSystem> playSFXListener;
			Events::GlobalEventListener<AudioSystem> playSFXRandomPitchListener;
			Events::GlobalEventListener<AudioSystem> stopSFXListener;
			Events::GlobalEventListener<AudioSystem> playMusicListener;
			Events::GlobalEventListener<AudioSystem> stopMusicListener;
			Events::GlobalEventListener<AudioSystem> loadSoundListener;
			Events::GlobalEventListener<AudioSystem> streamMusicListener;
			FMOD_RESULT result;
			FMOD::System *system;

			FMOD::Sound *sfx;
			FMOD::Sound *music;

			FMOD::Channel *currentMusicChannel;
			FMOD::Channel *nextMusicChannel;
			FMOD::Channel *sfxChannel;

			FMOD::ChannelGroup *sfxGroup;
			FMOD::ChannelGroup *musicGroup;

			std::unordered_map<std::string, FMOD::Sound *> soundsMap;
			std::unordered_map<std::string, FMOD::Sound *> musicMap;

			bool isCrossfading = false;
			float crossfadeDuration = 2.0f;
			float crossfadeTimer = 0.0f;
			std::string currentMusicName;

			// Whether music channel group is muted (SFX unaffected)
			bool musicMuted_{false};
	};
}

