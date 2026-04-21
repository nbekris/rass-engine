// File Name:    PauseMenuSystem.h
// Author(s):    main Niko Bekris, secondary Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      System managing the pause menu UI.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <initializer_list>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_set>

#include "Events/GlobalEventArgs.h"
#include "Events/GlobalEventListener.h"
#include "IEvent.h"
#include "IPauseMenuSystem.h"

namespace RassEngine::Systems {

class PauseMenuSystem : public IPauseMenuSystem {
public:
	enum class PauseSubMenu { Main, Options, ConfirmExit, HowToPlay };

	PauseMenuSystem();
	virtual ~PauseMenuSystem() override;

	// Inherited via IPauseMenuSystem
	bool Initialize() override;
	const std::string_view &NameClass() const override;
	void Shutdown() override;

	bool IsPaused() const override { return isPaused; }
	void Resume() override;
	void ShowOptions() override;
	void ShowHowToPlay() override;
	void BackToPause() override;
	void ShowConfirmExit() override;

private:
	PauseMenuSystem(const PauseMenuSystem &) = delete;
	PauseMenuSystem(PauseMenuSystem &&) noexcept = delete;
	PauseMenuSystem &operator=(const PauseMenuSystem &) = delete;
	PauseMenuSystem &operator=(PauseMenuSystem &&) noexcept = delete;

	bool Update(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &);
	bool Render(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &);
	bool OnPause(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &);

	void Pause();
	void SpawnPauseEntities();
	void DestroyPauseEntities();
	void SpawnOptionsEntities();
	void DestroyOptionsEntities();
	void SpawnConfirmExitEntities();
	void DestroyConfirmExitEntities();
	void SpawnHowToPlayEntities();
	void DestroyHowToPlayEntities();
	bool IsMenuScene() const;

	Events::GlobalEventListener<PauseMenuSystem> updateListener;
	Events::GlobalEventListener<PauseMenuSystem> renderListener;
	Events::GlobalEventListener<PauseMenuSystem> onPauseListener;

	bool isPaused{false};
	float savedTimeScale{1.0f};
	PauseSubMenu currentSubMenu{PauseSubMenu::Main};
	std::vector<std::string> pauseEntityNames;
	std::vector<std::string> optionsEntityNames;
	std::vector<std::string> confirmExitEntityNames;
	std::vector<std::string> howToPlayEntityNames;
	std::unordered_set<std::string> scenePaths{};
};

}
