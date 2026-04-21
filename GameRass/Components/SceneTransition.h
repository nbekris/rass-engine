// File Name:    SceneTransition.h
// Author(s):    main Niko Bekris, secondary Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Component managing animated transitions between scenes.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <Cloneable.h>
#include <Component.h>
#include <Events/EventArgs.h>
#include <Events/EventListenerMethod.h>
#include <IEvent.h>
#include <Stream.h>
#include <string>
#include <string_view>

namespace RassGame::Components {

class SceneTransition : public RassEngine::Cloneable<RassEngine::Component, SceneTransition> {
public:
	SceneTransition();
	SceneTransition(const SceneTransition &other);
	virtual ~SceneTransition() override;

	virtual bool Initialize() override;
	virtual const std::string_view &NameClass() const override;
	virtual bool Read(RassEngine::Stream &stream) override;

	inline std::string_view GetNextScene() const {
		return nextScene;
	}
private:
	bool OnTriggerEnter(const RassEngine::IEvent<RassEngine::Events::EventArgs> *, const RassEngine::Events::EventArgs &args);
private:
	std::string nextScene{};
	std::string triggeredBy{"Player"};
	RassEngine::Events::EventListenerMethod<SceneTransition, RassEngine::Events::EventArgs> onTriggerEnter;
};

}
