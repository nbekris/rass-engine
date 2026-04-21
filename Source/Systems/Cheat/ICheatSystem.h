// File Name:    ICheatSystem.h
// Author(s):    main Taro Omiya, secondary Boyuan Hu, Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      System enabling developer cheat codes.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include "Systems/GlobalEvents/IGlobalSystem.h"

namespace RassEngine::Systems {

class ICheatSystem : public IGlobalSystem<ICheatSystem> {
public:
	virtual ~ICheatSystem() = default;

	/// <summary>
	/// Returns true if god mode is currently active.
	/// Used by combat systems to apply 2x damage and skip player damage.
	/// </summary>
	virtual bool IsGodMode() const = 0;

protected:
	// Do not allow constructing an interface
	ICheatSystem() = default;

private:
	// Remove the rest of the default functions
	ICheatSystem(const ICheatSystem &) = delete;
	ICheatSystem(ICheatSystem &&) noexcept = delete;
	ICheatSystem &operator=(const ICheatSystem &) = delete;
	ICheatSystem &operator=(ICheatSystem &&) noexcept = delete;
};

}
