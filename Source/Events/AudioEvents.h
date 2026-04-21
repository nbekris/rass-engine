// File Name:    AudioEvents.h
// Author(s):    main Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Event definitions for audio system events.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <string>

namespace RassEngine::Events {

class GlobalEventID;

struct AudioEvents {
	// Event IDs
	static const GlobalEventID PlaySFXEvent;
	static const GlobalEventID PlaySFXRandomPitchEvent;
	static const GlobalEventID StopSFXEvent;
	static const GlobalEventID PlayMusicEvent;
	static const GlobalEventID StopMusicEvent;
	static const GlobalEventID LoadSoundEvent;
	static const GlobalEventID StreamMusicEvent;

	// Pending request data (written by helpers, read by AudioSystem handler)
	struct Request {
		std::string soundName;
		float volume = 1.0f;
		bool isLooped = false;
		float minPitch = 0.8f;
		float maxPitch = 1.2f;
	};
	static Request pendingRequest;

	// Fire-and-forget helpers (set pending args + dispatch event)
	static void PlaySFX(const std::string &soundName, float volume = 1.0f);
	static void PlaySFXRandomPitch(const std::string &soundName, float volume = 1.0f, float minPitch = 0.8f, float maxPitch = 1.2f);
	static void StopSFX(const std::string &soundName);
	static void PlayMusic(const std::string &soundName);
	static void StopMusic();
	static void LoadSound(const std::string &soundName);
	static void StreamMusic(const std::string &soundName, bool isLooped);
};

}
