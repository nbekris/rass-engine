// File Name:    ScreenFlashSystem.h
// Author(s):    main Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      System for flashing the screen.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include "IScreenFlashSystem.h"

#include <string_view>

#include <Components/SpriteFader.h>
#include <Events/GlobalEventArgs.h>
#include <Events/GlobalEventListener.h>
#include <IEvent.h>

namespace RassGame::Systems {
	class ScreenFlashSystem : public IScreenFlashSystem {
	public:
		ScreenFlashSystem();
		~ScreenFlashSystem() override;

		bool Initialize() override;
		const std::string_view &NameClass() const override; 
		void Shutdown() override;

		bool Show(const glm::vec3 &color, const RassEngine::TweenCurve &curve) override;

	private:
		bool OnSceneLoaded(const RassEngine::IEvent<RassEngine::Events::GlobalEventArgs> *, const RassEngine::Events::GlobalEventArgs &);
		bool OnSceneUnloaded(const RassEngine::IEvent<RassEngine::Events::GlobalEventArgs> *, const RassEngine::Events::GlobalEventArgs &);

		static constexpr std::string_view ENTITY_NAME = "ScreenFlash";
		RassEngine::Components::SpriteFader* flashComponent{nullptr};
		RassEngine::Events::GlobalEventListener<ScreenFlashSystem> onSceneLoaded, onSceneUnloaded;
	};
}
