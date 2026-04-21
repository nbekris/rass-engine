// File Name:    Door.h
// Author(s):    main Niko Bekris, secondary Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Component controlling door open and close behavior.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <Cloneable.h>
#include <Component.h>
#include <Components/Collider.h>
#include <Components/PhysicsBody.h>
#include <Stream.h>
#include <string_view>
#include <unordered_set>
#include <UUID.h>

#include "Components/SpriteSwapper.h"
#include "Components/Switch.h"

namespace RassGame::Components {

class Door : public RassEngine::Cloneable<RassEngine::Component, Door> {
public:
	Door();
	Door(const Door &other);
	virtual ~Door() override;

	virtual bool Initialize() override;
	virtual const std::string_view &NameClass() const override;
	virtual bool Read(RassEngine::Stream &stream) override;

	inline bool IsOpen() const {
		return isOpen;
	}
public:
	enum class Type {
		Switch,
		Key
	};

	inline Type GetType() const {
		return type;
	}
private:
	Switch *FindSwitch(const RassEngine::UUID& id) const;
	void SetIsOpen(bool flag);
	bool OnSwitchChanged(const RassEngine::IEvent<Switch::ToggleEventArgs> *, const Switch::ToggleEventArgs &args);
private:
	Type type = Type::Switch;
	bool isOpen{false};
	std::unordered_set<RassEngine::UUID> bindTo{};
	SpriteSwapper *spriteSwapper{nullptr};
	RassEngine::Components::Collider *collider{nullptr};
	RassEngine::Components::PhysicsBody *physicsBody{nullptr};
	RassEngine::Events::EventListenerMethod<Door, Switch::ToggleEventArgs> onSwitchChanged;
};

}
