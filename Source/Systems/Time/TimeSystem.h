// File Name:    TimeSystem.h
// Author(s):    main Taro Omiya, secondary Niko Bekris, Boyuan Hu
// Course:       GAM541
// Project:      RASS
// Purpose:      System tracking delta time and fixed update intervals.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <chrono>
#include <ratio>
#include <string_view>

#include "ITimeSystem.h"

using namespace std::chrono;

// forward declation
namespace RassEngine::Tests::Systems {
class TestTimeSystem_TestStartTrackingTime_Test;
class TestTimeSystem_TestDefaultFrameLocking_Test;
}

namespace RassEngine::Systems {

class TimeSystem : public ITimeSystem {
	// Allow unit tests access to this class
	friend class RassEngine::Tests::Systems::TestTimeSystem_TestStartTrackingTime_Test;
	friend class RassEngine::Tests::Systems::TestTimeSystem_TestDefaultFrameLocking_Test;
public:
	static constexpr float DEFAULT_FPS = 60.f;
	static constexpr float DEFAULT_FIXED_DELTA_TIME = 0.02f;
	static constexpr float DEFAULT_TIME_SCALE = 1.0f;

	// Inherited via ITimeSystem
	bool Initialize() override;
	const std::string_view &NameClass() const override;
	void Shutdown() override;

	inline float GetDeltaTimeSec() const override {
		return currentDeltaTime;
	}
	inline float GetUnscaledDeltaTime() const override {
		return currentUnscaledDeltaTime;
	}

	inline float GetTimePassedSec() const override {
		return timePassedScaled;
	}
	float GetUnscaledTimePassedSec() const override;

	float GetCurrentFPS() const override;

	float GetTargetFrameRate() const;
	void SetTargetFrameRate(float frameRate) override;

	float GetFixedDeltaTimeSec() const override;
	inline float GetUnscaledFixedDeltaTime() const override {
		return unscaledFixedDeltaTime;
	}
	inline void SetFixedDeltaTimeSec(float duration) override {
		unscaledFixedDeltaTime = duration;
	}

	inline float GetTimeScale() const override {
		return timeScale;
	}
	inline void SetTimeScale(float timeScale) override {
		this->timeScale = timeScale;
	}

protected:
	void StartTrackingTime() override;
	void EndFrame() override;
	bool UpdateDeltaTime() override;

private:
	// The maximum duration to wait each frame
	duration<float> targetSleepDuration{};
	// The timestamp when the game started
	steady_clock::time_point gameStartTime{};
	// Timestamp when frame started
	steady_clock::time_point frameStartTime{};
	float unscaledFixedDeltaTime{DEFAULT_FIXED_DELTA_TIME};
	float timeScale{DEFAULT_TIME_SCALE};
	float currentDeltaTime{0.f};
	float currentUnscaledDeltaTime{0.f};
	float timePassedScaled{0.f};

	// Cache values
	float lastUnscaledDeltaTime{0.f};
	float timeSinceFixedUpdateCalled{0.f};

	// Helper variables to set the frame rate
	time_point<steady_clock, duration<float, std::nano>> targetFrameEndTime{};

#ifdef _DEBUG
	// Helper Methods
	void CalculateFPS();

	// Helper variables to calculate FPS
	float lastFPS{0.f};
	unsigned int numFrames{0};
	float cumulativeDuration{0.f};
#endif // _DEBUG
};

}
