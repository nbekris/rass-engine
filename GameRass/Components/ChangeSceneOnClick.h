// File Name:    ChangeSceneOnClick.h
// Author(s):    main Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Component that transitions to a new scene on click.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <string_view>
#include <string>

#include <Cloneable.h>
#include <Component.h>
#include <Stream.h>
#include <IEvent.h>
#include <Events/EventArgs.h>
#include <Events/EventListenerMethod.h>

namespace RassGame::Components {

class ChangeSceneOnClick : public RassEngine::Cloneable<RassEngine::Component, ChangeSceneOnClick> {
public:
	ChangeSceneOnClick();
	ChangeSceneOnClick(const ChangeSceneOnClick &other);
	virtual ~ChangeSceneOnClick() override;

	virtual bool Initialize() override;
	virtual const std::string_view &NameClass() const override;
	virtual bool Read(RassEngine::Stream &stream) override;

private:
	bool OnClick(const RassEngine::IEvent<RassEngine::Events::EventArgs> *, const RassEngine::Events::EventArgs &);

	std::string sceneName{};
	RassEngine::Events::EventListenerMethod<ChangeSceneOnClick, RassEngine::Events::EventArgs> onClick;
};

}
