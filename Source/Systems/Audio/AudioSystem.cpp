// File Name:    AudioSystem.cpp
// Author(s):    main Niko Bekris, secondary Taro Omiya, Boyuan Hu
// Course:       GAM541
// Project:      RASS
// Purpose:      System managing audio playback and mixing.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "AudioSystem.h"

#include <fmod_errors.h>
#include <random>

#include "Events/AudioEvents.h"
#include "Events/Global.h"
#include "Systems/GlobalEvents/IGlobalEventsSystem.h"
#include "Systems/Logging/ILoggingSystem.h"
#include "Systems/Resource/IResourceSystem.h"
#include "Systems/Time/ITimeSystem.h"
#include "Utils.h"

namespace RassEngine::Systems {
	static constexpr std::string_view AUDIO_FOLDER = "Audio";

	AudioSystem::AudioSystem()
		: updateListener{this, &AudioSystem::Update},
		playSFXListener{this, &AudioSystem::OnPlaySFX},
		playSFXRandomPitchListener{this, &AudioSystem::OnPlaySFXRandomPitch},
		stopSFXListener{this, &AudioSystem::OnStopSFX},
		playMusicListener{this, &AudioSystem::OnPlayMusic},
		stopMusicListener{this, &AudioSystem::OnStopMusic},
		loadSoundListener{this, &AudioSystem::OnLoadSound},
		streamMusicListener{this, &AudioSystem::OnStreamMusic},

		result(FMOD_OK),
		sfx(nullptr),
		music(nullptr),
		currentMusicChannel(nullptr),
		nextMusicChannel(nullptr),
		sfxChannel(nullptr),
		sfxGroup(nullptr),
		musicGroup(nullptr) {

		if(IGlobalEventsSystem::Get() != nullptr) {
			IGlobalEventsSystem::Get()->bind(Events::Global::Update, &updateListener);
			IGlobalEventsSystem::Get()->bind(Events::AudioEvents::PlaySFXEvent, &playSFXListener);
			IGlobalEventsSystem::Get()->bind(Events::AudioEvents::PlaySFXRandomPitchEvent, &playSFXRandomPitchListener);
			IGlobalEventsSystem::Get()->bind(Events::AudioEvents::StopSFXEvent, &stopSFXListener);
			IGlobalEventsSystem::Get()->bind(Events::AudioEvents::PlayMusicEvent, &playMusicListener);
			IGlobalEventsSystem::Get()->bind(Events::AudioEvents::StopMusicEvent, &stopMusicListener);
			IGlobalEventsSystem::Get()->bind(Events::AudioEvents::LoadSoundEvent, &loadSoundListener);
			IGlobalEventsSystem::Get()->bind(Events::AudioEvents::StreamMusicEvent, &streamMusicListener);
		}

		result = FMOD_OK;
		system = nullptr;
		soundsMap.reserve(100);
		musicMap.reserve(100);
	}

	AudioSystem::~AudioSystem() {
		if(IGlobalEventsSystem::Get() != nullptr) {
			IGlobalEventsSystem::Get()->unbind(Events::Global::Update, &updateListener);
			IGlobalEventsSystem::Get()->unbind(Events::AudioEvents::PlaySFXEvent, &playSFXListener);
			IGlobalEventsSystem::Get()->unbind(Events::AudioEvents::PlaySFXRandomPitchEvent, &playSFXRandomPitchListener);
			IGlobalEventsSystem::Get()->unbind(Events::AudioEvents::StopSFXEvent, &stopSFXListener);
			IGlobalEventsSystem::Get()->unbind(Events::AudioEvents::PlayMusicEvent, &playMusicListener);
			IGlobalEventsSystem::Get()->unbind(Events::AudioEvents::StopMusicEvent, &stopMusicListener);
			IGlobalEventsSystem::Get()->unbind(Events::AudioEvents::LoadSoundEvent, &loadSoundListener);
			IGlobalEventsSystem::Get()->unbind(Events::AudioEvents::StreamMusicEvent, &streamMusicListener);
		}
		ShutDownAudio();
	}

	bool AudioSystem::Initialize() {
		if(IGlobalEventsSystem::Get() == nullptr) {
			if(Systems::ILoggingSystem::Get()) {
				LOG_WARNING("Could not initialize IGlobal Events System", NAMEOF(AudioSystem));
				return false;
			}
		}

		result = FMOD::System_Create(&system);
		CheckResult(result);

		result = system->init(512, FMOD_INIT_NORMAL, 0);
		CheckResult(result);

		system->createChannelGroup("SFXGroup", &sfxGroup);
		sfxGroup->setVolume(0.3f);

		system->createChannelGroup("MusicGroup", &musicGroup);
		musicGroup->setVolume(0.3f);
		return true;
	}

	void AudioSystem::Shutdown() {
		ShutDownAudio();
	}

	const std::string_view &AudioSystem::NameClass() const {
		static constexpr std::string_view className = NAMEOF(AudioSystem);
		return className;
	}

	bool AudioSystem::Update(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &args) {
		float dt = Systems::ITimeSystem::Get()->GetDeltaTimeSec();

		if(system) {
			result = system->update();
			CheckResult(result);

			if(isCrossfading) CrossfadeUpdate(dt);
		}
		return true;
	}

	void RassEngine::Systems::AudioSystem::LoadSound(const std::string_view &fileName) {
		// Make sure the SFX has not been already loaded
		std::string filePath = IResourceSystem::Path(AUDIO_FOLDER, fileName);
		if(soundsMap.contains(filePath)) {
			return;
		}

		result = system->createSound(filePath.c_str(), FMOD_NONBLOCKING, 0, &sfx);

		soundsMap.insert({filePath, sfx});
		CheckResult(result);
	}

	void RassEngine::Systems::AudioSystem::StreamMusic(const std::string_view &fileName, bool isLooped) {
		std::string filePath = IResourceSystem::Path(AUDIO_FOLDER, fileName);
		if(musicMap.contains(filePath)) {
			return;
		}

		FMOD_MODE mode = isLooped ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
		result = system->createStream(filePath.c_str(), mode, 0, &music);
		CheckResult(result);

		musicMap.insert({filePath, music});
	}

	void RassEngine::Systems::AudioSystem::PlaySFX(const std::string_view &soundName, float volume) {
		std::string filePath = IResourceSystem::Path(AUDIO_FOLDER, soundName);
		auto it = soundsMap.find(filePath);
		if(it != soundsMap.end()) {
			system->playSound(it->second, nullptr, true, &sfxChannel);

			if(sfxChannel) {
				sfxChannel->setChannelGroup(sfxGroup);
				sfxChannel->setPaused(false);
			}
		}
	}

	void RassEngine::Systems::AudioSystem::PlaySFXRandomPitch(const std::string_view &soundName, float volume, float minPitch, float maxPitch) {
		std::string filePath = IResourceSystem::Path(AUDIO_FOLDER, soundName);
		auto it = soundsMap.find(filePath);
		if(it != soundsMap.end()) {
			system->playSound(it->second, nullptr, true, &sfxChannel);

			if(sfxChannel) {
				sfxChannel->setChannelGroup(sfxGroup);

				static std::mt19937 rng{std::random_device{}()};
				std::uniform_real_distribution<float> dist(minPitch, maxPitch);
				sfxChannel->setPitch(dist(rng));

				sfxChannel->setPaused(false);
			}
		}
	}

	void RassEngine::Systems::AudioSystem::PlayMusic(const std::string_view &name) {
		std::string filePath = IResourceSystem::Path(AUDIO_FOLDER, name);
		if(!IsChannelPlaying(currentMusicChannel) && !isCrossfading) {
			auto it = musicMap.find(filePath);
			if(it != musicMap.end()) {
				result = system->playSound(it->second, musicGroup, false, &currentMusicChannel);
				currentMusicName = filePath;
			}
		} else if(IsChannelPlaying(currentMusicChannel)) {
			Crossfade(filePath, 0.5f);
		}
		CheckResult(result);
	}

	void AudioSystem::StopMusic() {
		if(IsChannelPlaying(currentMusicChannel)) {
			result = currentMusicChannel->stop();
		}
		CheckResult(result);
	}

	void RassEngine::Systems::AudioSystem::StopSFX(const std::string_view &soundName) {
		if(IsChannelPlaying(sfxChannel)) {
			result = sfxChannel->stop();
		}
		CheckResult(result);
	}

	void AudioSystem::Crossfade(std::string name, float duration) {
		if(isCrossfading) {
			//TODO : Handle already crossfading case
			return;
		}

		if(currentMusicName == name) return;

		FMOD::Sound *nextMusic = nullptr;
		auto it = musicMap.find(name);
		if(it != musicMap.end()) {
			nextMusic = musicMap[name];
		} else {
			return;
		}

		result = system->playSound(nextMusic, musicGroup, true, &nextMusicChannel);
		CheckResult(result);
		currentMusicName = name;

		if(nextMusicChannel) {
			nextMusicChannel->setVolume(0.0f);
			nextMusicChannel->setPaused(false);
		}

		crossfadeDuration = duration;
		crossfadeTimer = 0.0f;
		isCrossfading = true;
	}

	bool AudioSystem::IsChannelPlaying(FMOD::Channel *channel) {
		if(!channel) return false;
		bool isPlaying = false;
		FMOD_RESULT res = channel->isPlaying(&isPlaying);
		if(res == FMOD_ERR_INVALID_HANDLE || res == FMOD_ERR_CHANNEL_STOLEN) {
			return false;
		}
		return isPlaying;
	}

	void AudioSystem::ShutDownAudio() {
		for(auto &pair : soundsMap) {
			if(pair.second) {
				pair.second->release();
			}
		}

		soundsMap.clear();

		if(sfx) {
			sfx->release();
			sfx = nullptr;
		}

		if(music) {
			music->release();
			music = nullptr;
		}

		currentMusicChannel = nullptr;
		sfxChannel = nullptr;

		if(system) {
			system->close();
			system->release();
			system = nullptr;
		}
	}

	void AudioSystem::CheckResult(FMOD_RESULT result) {
		if(result != FMOD_OK) {
			printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
			exit(-1);
		}
	}

	// Audio event handlers
	bool AudioSystem::OnPlaySFX(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &) {
		auto &req = Events::AudioEvents::pendingRequest;
		PlaySFX(req.soundName, req.volume);
		return true;
	}

	bool AudioSystem::OnPlaySFXRandomPitch(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &) {
		auto &req = Events::AudioEvents::pendingRequest;
		PlaySFXRandomPitch(req.soundName, req.volume, req.minPitch, req.maxPitch);
		return true;
	}

	bool AudioSystem::OnStopSFX(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &) {
		auto &req = Events::AudioEvents::pendingRequest;
		StopSFX(req.soundName);
		return true;
	}

	bool AudioSystem::OnPlayMusic(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &) {
		auto &req = Events::AudioEvents::pendingRequest;
		PlayMusic(req.soundName);
		return true;
	}

	bool AudioSystem::OnStopMusic(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &) {
		StopMusic();
		return true;
	}

	bool AudioSystem::OnLoadSound(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &) {
		auto &req = Events::AudioEvents::pendingRequest;
		LoadSound(req.soundName);
		return true;
	}

	bool AudioSystem::OnStreamMusic(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &) {
		auto &req = Events::AudioEvents::pendingRequest;
		StreamMusic(req.soundName, req.isLooped);
		return true;
	}

	void AudioSystem::CrossfadeUpdate(float dt) {
		crossfadeTimer += dt;

		float t = crossfadeTimer / crossfadeDuration;

		if(t >= 1.0f) {
			t = 1.0f;
			isCrossfading = false;

			if(currentMusicChannel) {
				currentMusicChannel->stop();
			}

			currentMusicChannel = nextMusicChannel;
			nextMusicChannel = nullptr;

			if(currentMusicChannel) {
				currentMusicChannel->setVolume(1.0f);
			}
		} else {
			if(currentMusicChannel) {
				float volOut = std::cos(t * (3.14159f / 2.0f));
				currentMusicChannel->setVolume(volOut);
			}

			if(nextMusicChannel) {
				float volIn = std::sin(t * (3.14159f / 2.0f));
				nextMusicChannel->setVolume(volIn);
			}
		}
	}

	void AudioSystem::SetMusicMuted(bool muted) {
		musicMuted_ = muted;
		if(musicGroup) {
			musicGroup->setMute(muted);
		}
	}

	bool AudioSystem::IsMusicMuted() const {
		return musicMuted_;
	}

	void AudioSystem::SetMusicVolume(float volume) {
		volume = std::clamp(volume, 0.0f, 1.0f);
		if(musicGroup) {
			musicGroup->setVolume(volume);
		}
	}

	float AudioSystem::GetMusicVolume() const {
		float volume = 0.0f;
		if(musicGroup) {
			musicGroup->getVolume(&volume);
		}
		return volume;
	}

	void AudioSystem::SetSFXVolume(float volume) {
		volume = std::clamp(volume, 0.0f, 1.0f);
		if(sfxGroup) {
			sfxGroup->setVolume(volume);
		}
	}

	float AudioSystem::GetSFXVolume() const {
		float volume = 0.0f;
		if(sfxGroup) {
			sfxGroup->getVolume(&volume);
		}
		return volume;
	}
}
