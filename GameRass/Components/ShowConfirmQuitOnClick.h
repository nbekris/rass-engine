// File Name:    ShowConfirmQuitOnClick.h
// Author(s):    Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Component that shows a confirm quit prompt on the start menu.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <string>
#include <string_view>
#include <vector>

#include <Cloneable.h>
#include <Component.h>
#include <Stream.h>
#include <IEvent.h>
#include <Events/EventArgs.h>
#include <Events/EventListenerMethod.h>
#include <Events/GlobalEventArgs.h>
#include <Events/GlobalEventListener.h>

namespace RassGame::Components {

class ShowConfirmQuitOnClick : public RassEngine::Cloneable<RassEngine::Component, ShowConfirmQuitOnClick> {
public:
	ShowConfirmQuitOnClick();
	ShowConfirmQuitOnClick(const ShowConfirmQuitOnClick &other);
	virtual ~ShowConfirmQuitOnClick() override;

	virtual bool Initialize() override;
	virtual const std::string_view &NameClass() const override;
	virtual bool Read(RassEngine::Stream &stream) override;

private:
	bool OnClick(const RassEngine::IEvent<RassEngine::Events::EventArgs> *, const RassEngine::Events::EventArgs &);
	bool OnRender(const RassEngine::IEvent<RassEngine::Events::GlobalEventArgs> *, const RassEngine::Events::GlobalEventArgs &);

	RassEngine::Events::EventListenerMethod<ShowConfirmQuitOnClick, RassEngine::Events::EventArgs> onClick;
	RassEngine::Events::GlobalEventListener<ShowConfirmQuitOnClick> renderListener;

	bool isShowingConfirmQuit{false};
	std::vector<std::string> spawnedEntityNames;
};

}
