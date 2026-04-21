// File Name:    ShowConfirmExitOnClick.h
// Author(s):    main Taro Omiya, secondary Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Component that shows the confirm-exit dialog on click.
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

class ShowConfirmExitOnClick : public RassEngine::Cloneable<RassEngine::Component, ShowConfirmExitOnClick> {
public:
	ShowConfirmExitOnClick();
	ShowConfirmExitOnClick(const ShowConfirmExitOnClick &other);
	virtual ~ShowConfirmExitOnClick() override;

	virtual bool Initialize() override;
	virtual const std::string_view &NameClass() const override;
	virtual bool Read(RassEngine::Stream &stream) override;

private:
	bool OnClick(const RassEngine::IEvent<RassEngine::Events::EventArgs> *, const RassEngine::Events::EventArgs &);

	RassEngine::Events::EventListenerMethod<ShowConfirmExitOnClick, RassEngine::Events::EventArgs> onClick;
};

}
