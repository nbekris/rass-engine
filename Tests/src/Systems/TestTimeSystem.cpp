// File Name:    TestTimeSystem.cpp
// Author(s):    main Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Unit tests for the TimeSystem.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "pch.h"

#include "Systems/Time/TimeSystem.h"

using namespace RassEngine::Systems;

namespace RassEngine::Tests::Systems {

TEST(TestTimeSystem, TestStartTrackingTime) {
	// Setup the test class
	TimeSystem testSystem{};
	testSystem.Initialize();

	// Run the method in question
	testSystem.StartTrackingTime();

	// Affirm defaults
	EXPECT_FLOAT_EQ(TimeSystem::DEFAULT_FPS, testSystem.GetTargetFrameRate());
	EXPECT_FLOAT_EQ(TimeSystem::DEFAULT_FIXED_DELTA_TIME, testSystem.GetFixedDeltaTimeSec());
	EXPECT_FLOAT_EQ(TimeSystem::DEFAULT_FIXED_DELTA_TIME, testSystem.GetUnscaledFixedDeltaTime());
	EXPECT_FLOAT_EQ(TimeSystem::DEFAULT_TIME_SCALE, testSystem.GetTimeScale());

	// Affirm delta time
	EXPECT_FLOAT_EQ(0.f, testSystem.GetDeltaTimeSec());
	EXPECT_FLOAT_EQ(0.f, testSystem.GetUnscaledDeltaTime());
}

TEST(TestTimeSystem, TestDefaultFrameLocking) {
	// Setup the test class
	TimeSystem testSystem{};
	testSystem.Initialize();

	// Run the method in question
	testSystem.StartTrackingTime();
	float targetDeltaTime = 1.0f / testSystem.GetTargetFrameRate();

	// Get the current time
	auto frameStartTime = steady_clock::now();

	// Lock the frame rate
	testSystem.EndFrame();

	// Get the new time
	auto frameEndTime = steady_clock::now();

	// Affirm locked time is longer than the target frame rate
	float actualDeltaTime = duration<float>(frameEndTime - frameStartTime).count();
	EXPECT_GE(actualDeltaTime, targetDeltaTime);

	// Make sure the delta times are reported accurately, as well
	EXPECT_GE(testSystem.GetDeltaTimeSec(), targetDeltaTime);
	EXPECT_GE(testSystem.GetUnscaledDeltaTime(), targetDeltaTime);
}

}
