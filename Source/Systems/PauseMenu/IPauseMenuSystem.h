// File Name:    IPauseMenuSystem.h
// Author(s):    main Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      System managing the pause menu UI.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include "Systems/GlobalEvents/IGlobalSystem.h"

namespace RassEngine::Systems {

class IPauseMenuSystem : public IGlobalSystem<IPauseMenuSystem> {
public:
	virtual ~IPauseMenuSystem() = default;

	/// <summary>
	/// Returns true if the game is currently paused via the pause menu.
	/// </summary>
	virtual bool IsPaused() const = 0;

	/// <summary>
	/// Resumes the game from the pause menu.
	/// </summary>
	virtual void Resume() = 0;

	/// <summary>
	/// Switches from the pause menu to the options overlay.
	/// </summary>
	virtual void ShowOptions() = 0;

	/// <summary>
	/// Switches from the pause menu to the how-to-play overlay.
	/// </summary>
	virtual void ShowHowToPlay() = 0;

	/// <summary>
	/// Switches from a sub-overlay (options or how-to-play) back to the pause menu.
	/// </summary>
	virtual void BackToPause() = 0;

	/// <summary>
	/// Switches from the pause menu to the confirm exit overlay.
	/// </summary>
	virtual void ShowConfirmExit() = 0;

protected:
	IPauseMenuSystem() = default;

private:
	IPauseMenuSystem(const IPauseMenuSystem &) = delete;
	IPauseMenuSystem(IPauseMenuSystem &&) noexcept = delete;
	IPauseMenuSystem &operator=(const IPauseMenuSystem &) = delete;
	IPauseMenuSystem &operator=(IPauseMenuSystem &&) noexcept = delete;
};

}
