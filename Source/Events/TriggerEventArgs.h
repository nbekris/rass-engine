// File Name:    TriggerEventArgs.h
// Author(s):    main Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Event argument data for trigger collision events.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include "EventArgs.h"

namespace RassEngine {
	class Entity;
}

namespace RassEngine::Events {

struct TriggerEventArgs : public EventArgs {
	enum class TriggerEventType {
		OnTriggerEnter,
		OnTriggerStay,
		OnTriggerExit
	};

	TriggerEventArgs(Entity* _otherEntity, TriggerEventType _eventType)
		: EventArgs{}
		, otherEntity{_otherEntity}
		, eventType{_eventType} {}

	virtual ~TriggerEventArgs() = default;

	Entity* otherEntity;
	TriggerEventType eventType;

private:
	TriggerEventArgs() = delete;
	TriggerEventArgs(const TriggerEventArgs& other) = delete;
	TriggerEventArgs(TriggerEventArgs&& other) noexcept = delete;
	TriggerEventArgs& operator=(const TriggerEventArgs&) = delete;
	TriggerEventArgs& operator=(TriggerEventArgs&&) noexcept = delete;
};

}
