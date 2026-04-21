// File Name:    ShowOptionsOnClick.h
// Author(s):    main Taro Omiya, secondary Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Component that opens the options menu on click.
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

class ShowOptionsOnClick : public RassEngine::Cloneable<RassEngine::Component, ShowOptionsOnClick> {
public:
	ShowOptionsOnClick();
	ShowOptionsOnClick(const ShowOptionsOnClick &other);
	virtual ~ShowOptionsOnClick() override;

	virtual bool Initialize() override;
	virtual const std::string_view &NameClass() const override;
	virtual bool Read(RassEngine::Stream &stream) override;

private:
	bool OnClick(const RassEngine::IEvent<RassEngine::Events::EventArgs> *, const RassEngine::Events::EventArgs &);

	RassEngine::Events::EventListenerMethod<ShowOptionsOnClick, RassEngine::Events::EventArgs> onClick;
};

}
