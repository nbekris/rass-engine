// File Name:    ITimeSystem.h
// Author(s):    main Taro Omiya, secondary Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      System tracking delta time and fixed update intervals.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include "../GlobalEvents/IGlobalSystem.h"

// Forward declaration
namespace RassEngine {
class Engine;
}

namespace RassEngine::Systems {

class ITimeSystem : public IGlobalSystem<ITimeSystem> {
public:
	virtual ~ITimeSystem() = default;

	/// <summary>
	/// Gets the time passed between each global event calls.
	/// For Update events, this will be set to the duration between each frame.
	/// For FixedUpdate events, this will be set to the fixed delta time.
	/// Units are in seconds.
	/// </summary>
	/// <seealso cref="GetFixedDeltaTimeSec"/>
	virtual float GetDeltaTimeSec() const = 0;
	virtual float GetUnscaledDeltaTime() const = 0;
	/// <summary>
	/// Gets the number of seconds that has passed since the game has started.
	/// </summary>
	virtual float GetTimePassedSec() const = 0;
	virtual float GetUnscaledTimePassedSec() const = 0;
	/// <summary>
	/// If in debug mode, provides the current frames-per-second.
	/// Otherwise, returns -1.
	/// </summary>
	virtual float GetCurrentFPS() const = 0;

	/// <summary>
	/// Gets the current target set for frame rate.
	/// The actual frame rate may be lower based on performance.
	/// </summary>
	virtual float GetTargetFrameRate() const = 0;
	/// <summary>
	/// Sets the target frame rate to run the game.
	/// </summary>
	virtual void SetTargetFrameRate(float frameRate) = 0;

	/// <summary>
	/// Gets the delta time between each FixedUpdate event call.
	/// </summary>
	virtual float GetFixedDeltaTimeSec() const = 0;
	virtual float GetUnscaledFixedDeltaTime() const = 0;
	/// <summary>
	/// Sets the delta time between each FixedUpdate event call.
	/// The set value is *not* affected by time scale.
	/// </summary>
	virtual void SetFixedDeltaTimeSec(float duration) = 0;

	virtual float GetTimeScale() const = 0;
	virtual void SetTimeScale(float timeScale) = 0;

protected:
	// Only allow the engine to access the following methods
	friend class Engine;

	/// <summary>
	/// Starts the process for computing how much time has passed in the current frame.
	/// </summary>
	virtual void StartTrackingTime() = 0;
	/// <summary>
	/// Ends the process for computing how much time has passed in the current frame.
	/// This method should compute the final performance
	/// </summary>
	virtual void EndFrame() = 0;
	/// <summary>
	/// Updates delta time to either FixedDeltaTime
	/// or actual time lapsed since last frame.
	/// </summary>
	/// <returns>
	/// True, if DeltaTime is set to FixedDeltaTime.
	/// </returns>
	virtual bool UpdateDeltaTime() = 0;
};

}
