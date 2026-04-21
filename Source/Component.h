// File Name:    Component.h
// Author(s):    main Niko Bekris, secondary Taro Omiya, Steven Yacoub
// Course:       GAM541
// Project:      RASS
// Purpose:      Base component class for entity components.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include "Object.h"
#include "ICloneable.h"
#include "ISerializable.h"

namespace RassEngine {

// forward declaration
class Entity;
class Stream;

class Component : public Object, public ICloneable<Component>, public ISerializable<Stream> {
public:
	inline Component() : Object{}, isEnabled{true} {}
	virtual ~Component() override {};

	/// <summary>
	/// Note: Copy constructor does *not* copy over any
	/// data, so that clone would work properly.
	/// </summary>
	inline Component(const Component &other)
		: Object{other}, isEnabled{other.isEnabled}
	{}
	Component(Component &&other) noexcept;

	// Prevent the object from being set, normally.
	Component &operator=(const Component &) = delete;
	Component &operator=(Component &&) noexcept = delete;

public:
	virtual bool Read(Stream &stream) override;

	inline void Parent(Entity *parent) {
		this->parent = parent;
	}

	inline Entity *Parent() const {
		return parent;
	}

	inline bool IsEnabled() const {
		return isEnabled;
	}

	inline bool SetEnabled(bool flag) {
		return isEnabled = flag;
	}

private:
	Entity *parent{nullptr};
	bool isEnabled{true};
};

}
