// File Name:    Animation.h
// Author(s):    main Boyuan Hu, secondary Taro Omiya, Steven Yacoub
// Course:       GAM541
// Project:      RASS
// Purpose:      Component handling sprite animation playback.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <string_view>

#include "Component.h"
#include "Cloneable.h"
#include "Events/GlobalEventArgs.h"
#include "Events/GlobalEventListener.h"
#include "IEvent.h"

// Forward declaring json
namespace RassEngine {
class Stream;
}

namespace RassEngine::Components {

class Animation : public Cloneable<Component, Animation> {
public:
	Animation();
	Animation(const Animation& other);
	virtual ~Animation() override;

	virtual bool Initialize() override;
	virtual const std::string_view &NameClass() const override;

	virtual bool Read(Stream& stream) override;

	inline unsigned int FrameIndex() const {
		return frameIndex;
	}
	inline void FrameIndex(unsigned int index) {
		this->frameIndex = index;
	}

	inline unsigned int FrameCount() const {
		return frameCount;
	}
	inline void FrameCount(unsigned int count) {
		this->frameCount = count;
	}

	inline float FrameDuration() const {
		return frameDuration;
	}
	inline void FrameDuration(float duration) {
		this->frameDuration = duration;
	}

	inline float FrameDelay() const {
		return frameDelay;
	}
	inline void FrameDelay(float delay) {
		this->frameDelay = delay;
	}

	inline bool IsRunning() const {
		return isRunning;
	}
	inline void IsRunning(bool running) {
		this->isRunning = running;
	}

	inline bool IsLooping() const {
		return isLooping;
	}
	inline void IsLooping(bool looping) {
		this->isLooping = looping;
	}

	inline bool IsDone() const {
		return isDone;
	}

private:
	bool Update(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &);
	void AdvanceFrame();

		// @brief The current frame being displayed.
	unsigned int frameIndex{ 0 };

	// @brief The maximum number of frames in the sequence.
	unsigned int frameCount{1};

	// @brief The time remaining for the current frame.
	float frameDelay{0.0f};

	// @brief The amount of time to display each successive frame.
	float frameDuration{0.0f};

	// @brief True if the animation is running; false if the animation has stopped.
	bool isRunning{false};

	// @brief True if the animation loops back to the beginning.
	bool isLooping{false};

	// @brief True if the end of the animation sequence has been reached, false otherwise.
	// @brief (Hint: This applies to both looping and non-looping animations.)
	// @brief (Hint: This should be true for only one game loop.)
	bool isDone{false};

	Events::GlobalEventListener<Animation> onUpdateListener;
};
}
