// File Name:    Switch.h
// Author(s):    main Niko Bekris, secondary Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Component acting as a toggleable switch that controls doors.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <Cloneable.h>
#include <Component.h>
#include <Events/EventArgs.h>
#include <Events/EventListenerMethod.h>
#include <IEventListener.h>
#include <IEvent.h>
#include <memory>
#include <Entity.h>
#include <Stream.h>
#include <string>
#include <string_view>
#include <unordered_set>

#include "Components/SpriteSwapper.h"

namespace RassGame::Components {

class Switch : public RassEngine::Cloneable<RassEngine::Component, Switch> {
public:
	Switch();
	Switch(const Switch &other);
	virtual ~Switch() override;

	virtual bool Initialize() override;
	virtual const std::string_view &NameClass() const override;
	virtual bool Read(RassEngine::Stream &stream) override;
public:
	// Arguments for switch event
	class ToggleEventArgs : public RassEngine::Events::EventArgs {
		friend class Switch;
		inline ToggleEventArgs(const Switch &source)
			: RassEngine::Events::EventArgs{}
			, source{source}
		{}

	public:
		const Switch &source;
	};

	inline void Bind(RassEngine::IEventListener<ToggleEventArgs> *listener) {
		onSwitchChanged->bind(listener);
	}
	inline void Unbind(RassEngine::IEventListener<ToggleEventArgs> *listener) {
		onSwitchChanged->unbind(listener);
	}
public:
	enum class Type {
		TriggerOnce,
		Held,
		Toggle
	};

	inline Type GetType() const {
		return type;
	}
	bool IsOn() const;
private:
	bool OnTriggerEnter(const RassEngine::IEvent<RassEngine::Events::EventArgs> *, const RassEngine::Events::EventArgs &args);
	bool OnTriggerExit(const RassEngine::IEvent<RassEngine::Events::EventArgs> *, const RassEngine::Events::EventArgs &args);
	bool OnToggleChanged(const RassEngine::IEvent<ToggleEventArgs> *, const ToggleEventArgs &args);
	void SetIsOn(bool newIsOn);
private:
	static bool isToggleOn;
	static std::unique_ptr<RassEngine::IEvent<ToggleEventArgs>> eventOnToggleChanged;
private:
	Type type = Type::TriggerOnce;
	bool isOn{false};
	std::string switchAudioName;
	std::unordered_set<std::string> triggeredBy{};
	std::unordered_set<const RassEngine::Entity*> heldEntities{};
	std::unique_ptr<RassEngine::IEvent<ToggleEventArgs>> onSwitchChanged{};
	RassEngine::Events::EventListenerMethod<Switch, RassEngine::Events::EventArgs> onTriggerEnter, onTriggerExit;
	RassEngine::Events::EventListenerMethod<Switch, ToggleEventArgs> onToggleChanged;
	SpriteSwapper *spriteSwapper{nullptr};
};

}
