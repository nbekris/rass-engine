// File Name:    ZoomOutArea.h
// Author(s):    main Boyuan Hu
// Course:       GAM541
// Project:      RASS
// Purpose:      Component triggering a camera zoom-out in a defined area.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <string_view>

#include "Component.h"
#include "Cloneable.h"
#include "Events/EventArgs.h"
#include "Events/EventListenerMethod.h"
#include "IEvent.h"
#include "Stream.h"

namespace RassGame::Components {

class ZoomOutArea : public RassEngine::Cloneable<RassEngine::Component, ZoomOutArea> {
public:
	ZoomOutArea();
	ZoomOutArea(const ZoomOutArea &other);
	virtual ~ZoomOutArea() override;

	virtual bool Initialize() override;
	virtual const std::string_view &NameClass() const override;
	virtual bool Read(RassEngine::Stream &stream) override;

private:
	bool OnTriggerEnter(const RassEngine::IEvent<RassEngine::Events::EventArgs> *, const RassEngine::Events::EventArgs &);
	bool OnTriggerExit(const RassEngine::IEvent<RassEngine::Events::EventArgs> *, const RassEngine::Events::EventArgs &);
private:
	RassEngine::Events::EventListenerMethod<ZoomOutArea, RassEngine::Events::EventArgs> onTriggerEnter;
	RassEngine::Events::EventListenerMethod<ZoomOutArea, RassEngine::Events::EventArgs> onTriggerExit;
	float zoomOutValue{0.2f};
};

}
