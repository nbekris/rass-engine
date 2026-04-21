// File Name:    TimeSystem.cpp
// Author(s):    main Taro Omiya, secondary Boyuan Hu, Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      System tracking delta time and fixed update intervals.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "TimeSystem.h"

#include <thread>
#include <chrono>
#include <string_view>

#include "Utils.h"

#ifndef _DEBUG
#include "Systems/Logging/ILoggingSystem.h"
#endif
#include <timeapi.h>
#pragma comment(lib, "Winmm.lib")

using namespace std::chrono;

namespace RassEngine::Systems {

static constexpr float EPSILON = std::numeric_limits<float>::epsilon();

bool TimeSystem::Initialize() {
	// Request 1ms Windows timer resolution so sleep_until is accurate.
	// The default resolution is 15.625ms, which causes sleep_until to overshoot
	// by up to one full tick, collapsing 60fps to 30-40fps unpredictably.
	timeBeginPeriod(1);

	// Initialize all variables for now
	StartTrackingTime();

	// Compute how many nanoseconds to sleep
	SetTargetFrameRate(DEFAULT_FPS);
	SetFixedDeltaTimeSec(DEFAULT_FIXED_DELTA_TIME);
	SetTimeScale(DEFAULT_TIME_SCALE);
	return true;
}

void TimeSystem::Shutdown() {
	timeEndPeriod(1);
}

float TimeSystem::GetCurrentFPS() const {
#ifdef _DEBUG
	return lastFPS;
#else
	LOG_INFO("FPS is only calculated under Debug mode");
	return -1.f;
#endif // _DEBUG
}

float TimeSystem::GetUnscaledTimePassedSec() const {
	return duration<float>(steady_clock::now() - gameStartTime).count();
}

float TimeSystem::GetTargetFrameRate() const {
	return 1.0f / targetSleepDuration.count();
}

void TimeSystem::SetTargetFrameRate(float frameRate) {
	if(frameRate <= 0)
		targetSleepDuration = duration<float>::zero();
	// Compute how many nanoseconds to sleep
	else {
		targetSleepDuration = round<steady_clock::duration>(
			duration<float>{1.f / frameRate}
		);
	}
}

#ifdef _DEBUG
static constexpr float CALC_FPS_AFTER_SECONDS = 0.1f;
void TimeSystem::CalculateFPS() {
	// Increment number of frames
	++numFrames;
	cumulativeDuration += lastUnscaledDeltaTime;

	// Calculate the frame rate
	if(cumulativeDuration < CALC_FPS_AFTER_SECONDS) {
		return;
	}

	// Calculate the FPS
	lastFPS = 0;
	if(cumulativeDuration > EPSILON) {
		lastFPS = static_cast<float>(numFrames);
		lastFPS /= cumulativeDuration;
	}

	// Reset the rest of the variables
	numFrames = 0;
	cumulativeDuration = 0.f;
	//cumulativeDuration -= CALC_FPS_AFTER_SECONDS;
}
#endif

float TimeSystem::GetFixedDeltaTimeSec() const {
	// Make sure GetTimeScale() is not zero
	if(GetTimeScale() > EPSILON || GetTimeScale() < -EPSILON) {
		// Compute the fixed delta time
		return GetUnscaledFixedDeltaTime() / GetTimeScale();
	}

	// Otherwise, to avoid divide-by-zero error, return 0.
	return 0.f;
}

void TimeSystem::StartTrackingTime() {
	// Might need to change this since chrono is technically not a system level library
	// Track the timestamp, when the game began
	gameStartTime = steady_clock::now();
	frameStartTime = gameStartTime;

	// Calculate the timestamp we want to sleep for
	targetFrameEndTime = frameStartTime + targetSleepDuration;

	// Reset the frame count
	currentDeltaTime = 0.f;
	currentUnscaledDeltaTime = 0.f;
	timePassedScaled = 0.f;
	lastUnscaledDeltaTime = 0.f;
	timeSinceFixedUpdateCalled = 0.f;
#ifdef _DEBUG
	lastFPS = 0.f;
	numFrames = 0;
	cumulativeDuration = 0.f;
#endif // _DEBUG
}

void TimeSystem::EndFrame() {
	// First, sleep until the frame is over
	std::this_thread::sleep_until(targetFrameEndTime);

	// Capture the wake time once so that both the delta measurement and the next
	// target are based on the exact same instant — eliminates the two-now() gap
	// that would otherwise systematically under-count each frame's duration.
	const auto frameEndTime = steady_clock::now();

	// Calculate delta time
	lastUnscaledDeltaTime = duration<float>(frameEndTime - frameStartTime).count();

	// Increment cumulative play-time
	timeSinceFixedUpdateCalled += lastUnscaledDeltaTime;
	timePassedScaled += lastUnscaledDeltaTime * GetTimeScale();

	// Track the current frame's time
	frameStartTime = frameEndTime;

	// Advance the target by a fixed step from the PREVIOUS target, not from now.
	// This way a single sleep overshoot is automatically compensated by a shorter
	// sleep next frame, keeping the long-run average locked to the target rate.
	// If we have fallen behind by more than one frame (e.g. after a lag spike),
	// reset from now to avoid a burst of back-to-back uncapped frames.
	targetFrameEndTime += targetSleepDuration;
	if(targetFrameEndTime < frameEndTime) {
		targetFrameEndTime = frameEndTime + targetSleepDuration;
	}

#ifdef _DEBUG
	// Calculate FPS
	CalculateFPS();
#endif // _DEBUG
}

bool TimeSystem::UpdateDeltaTime() {
	// Check if we want to use the delta time
	if(timeSinceFixedUpdateCalled < GetUnscaledFixedDeltaTime()) {
		// If so, calculate delta time based on time scale
		currentUnscaledDeltaTime = lastUnscaledDeltaTime;
		currentDeltaTime = currentUnscaledDeltaTime * GetTimeScale();
		return false;
	}

	// Otherwise, use the fixed delta time
	currentUnscaledDeltaTime = GetUnscaledFixedDeltaTime();
	currentDeltaTime = GetFixedDeltaTimeSec();

	// Decrement the calls between fixed updates
	timeSinceFixedUpdateCalled -= GetUnscaledFixedDeltaTime();
	return true;
}

const std::string_view &TimeSystem::NameClass() const {
	static constexpr std::string_view className = NAMEOF(TimeSystem);
	return className;
}

}
