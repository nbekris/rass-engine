// File Name:    ShowHowToPlayOnClick.h
// Author(s):    main Taro Omiya, secondary Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Component that shows the how-to-play screen on click.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <string_view>

#include <Cloneable.h>
#include <Component.h>
#include <Stream.h>
#include <IEvent.h>
#include <Events/EventArgs.h>
#include <Events/EventListenerMethod.h>

namespace RassGame::Components {

class ShowHowToPlayOnClick : public RassEngine::Cloneable<RassEngine::Component, ShowHowToPlayOnClick> {
public:
	ShowHowToPlayOnClick();
	ShowHowToPlayOnClick(const ShowHowToPlayOnClick &other);
	virtual ~ShowHowToPlayOnClick() override;

	virtual bool Initialize() override;
	virtual const std::string_view &NameClass() const override;
	virtual bool Read(RassEngine::Stream &stream) override;

private:
	bool OnClick(const RassEngine::IEvent<RassEngine::Events::EventArgs> *, const RassEngine::Events::EventArgs &);

	RassEngine::Events::EventListenerMethod<ShowHowToPlayOnClick, RassEngine::Events::EventArgs> onClick;
};

}
