// File Name:    TimedSceneTransition.h
// Author(s):    main Niko Bekris, secondary Steven Yacoub
// Course:       GAM541
// Project:      RASS
// Purpose:      Component that transitions to a new scene after a delay.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <Cloneable.h>
#include <Component.h>
#include <Events/GlobalEventArgs.h>
#include <Events/GlobalEventListener.h>
#include <Systems/Input/IInputSystem.h>
#include <IEvent.h>
#include <Stream.h>
#include <string>
#include <string_view>
#include <Events/Update.h>

namespace RassGame::Components {

class TimedSceneTransition : public RassEngine::Cloneable<RassEngine::Component, TimedSceneTransition> {
public:
	TimedSceneTransition();
	TimedSceneTransition(const TimedSceneTransition &other);
	virtual ~TimedSceneTransition() override;

	virtual bool Initialize() override;
	virtual const std::string_view &NameClass() const override;
	virtual bool Read(RassEngine::Stream &stream) override;

	inline std::string_view GetNextScene() const {
		return nextScene;
	}

private:
	bool OnUpdate(const RassEngine::IEvent<RassEngine::Events::GlobalEventArgs> *, const RassEngine::Events::GlobalEventArgs &args);
	void bypassTransition(const std::string &sceneName);

private:
	float sceneDuration{2.0f};
	float currentTime{0.0f};
	std::string nextScene{};
	std::string sceneToBypassTo{};

	RassEngine::Systems::IInputSystem* inputSystem{nullptr};
	RassEngine::Events::GlobalEventListener<TimedSceneTransition> onUpdateListener;

};

}
