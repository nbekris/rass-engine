// File Name:    Object.h
// Author(s):    main Taro Omiya, secondary Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Base object class.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <string_view>
#include <type_traits>

#include "UUID.h"

namespace RassEngine {
// Forward declaration
class Entity;

class Object {
	friend struct std::hash<Object>;
	friend class Entity;
public:
	inline Object() : id{} {};
	inline virtual ~Object() {};

	/// <summary>
	/// Note: Copy constructor does *not* copy
	/// over the UUID, so that clone would work properly
	/// </summary>
	inline Object(const Object &) : id() {}
	inline Object(const Object &&other) noexcept : id(other.id) {}

	// Prevent the object from being set, normally.
	Object &operator=(const Object &) = delete;
	Object &operator=(Object &&) noexcept = delete;

	/// <summary>
	/// Called on initialization.
	/// </summary>
	/// <returns></returns>
	virtual bool Initialize() = 0;
	/// <summary>
	/// Gets the class name of the object class name
	/// Mostly useful for debugging.
	/// </summary>
	virtual const std::string_view& NameClass() const = 0;

	inline virtual bool operator==(const Object &other) const {
		return id == other.id;
	}

	inline virtual bool operator!=(const Object &other) const {
		return !operator==(other);
	}

	inline const UUID &GetID() const {
		return id;
	}

private:
	// Only Entity should be able to set the ID of this object
	UUID id;
};

}

// Define a hash functor for IObjects
template <>
struct std::hash<RassEngine::Object> {
	std::size_t operator()(const RassEngine::Object &k) const noexcept {
		return hash<RassEngine::UUID::Type>()(k.id);
	}
};
