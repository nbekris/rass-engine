// File Name:    CheatSystem.h
// Author(s):    main Taro Omiya, secondary Boyuan Hu, Steven Yacoub, Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      System enabling developer cheat codes.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

//#ifdef _DEBUG

#include <initializer_list>
#include <string>
#include <string_view>
#include <vector>

#include "Events/GlobalEventArgs.h"
#include "Events/GlobalEventListener.h"
#include "IEvent.h"
#include "ICheatSystem.h"

namespace RassEngine::Systems {

class CheatSystem : public ICheatSystem {
public:
	CheatSystem(const std::initializer_list<std::string_view> &scenePaths);
	virtual ~CheatSystem() override;

	// Inherited via ICheatSystem
	bool Initialize() override;
	const std::string_view &NameClass() const override;
	void Shutdown() override;

	/// <summary>
	/// Returns true if god mode is active (invulnerable + 2x damage).
	/// </summary>
	bool IsGodMode() const override {
		return godMode_;
	}

	void RenderCheatMenu();

private:
	// Remove the rest of the default functions
	CheatSystem(const CheatSystem &) = delete;
	CheatSystem(CheatSystem &&) noexcept = delete;
	CheatSystem &operator=(const CheatSystem &) = delete;
	CheatSystem &operator=(CheatSystem &&) noexcept = delete;

	/// <summary>
	/// Updates what keys are pressed
	/// </summary>
	/// <returns>True if update succeeds</returns>
	bool Update(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &);

	/// <summary>
	/// Renders the cheat menu ImGui window (called during Render event)
	/// </summary>
	/// <returns>True if render succeeds</returns>
	bool Render(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &);

	// Read update events
	Events::GlobalEventListener<CheatSystem> updateListener;
	// Read render events (ImGui must be called within frame scope)
	Events::GlobalEventListener<CheatSystem> renderListener;

	std::vector<std::string> scenePaths{};
	bool showCheatMenu{false};

	// ── Cheat states ──────────────────────────────────────────────────────
	/// God mode: player invulnerable + deals 2x damage
	bool godMode_{false};
	/// Mute music only (SFX unaffected)
	bool musicMuted_{false};
};

}
//#endif // _DEBUG
