// File Name:    EntityEventID.h
// Author(s):    main Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Event ID definitions for entity-level events.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <string_view>

#include "INamedEventID.h"

namespace RassEngine::Events {

class EntityEventID : INamedEventID {
public:
	/// <summary>
	/// Constructs an event ID with name
	/// </summary>
	EntityEventID(const std::string_view &name);
	inline virtual ~EntityEventID() = default;

	operator UUID::Type() const override;
	const UUID &GetID() const override;
	const std::string_view &GetName() const override;

	bool operator==(const EntityEventID &other) const;
	bool operator!=(const EntityEventID &other) const;

private:
	// Delete some defaults
	EntityEventID() = delete;
	EntityEventID(const EntityEventID &other) = delete;
	EntityEventID(const EntityEventID &&other) noexcept = delete;
	EntityEventID &operator=(const EntityEventID &other) = delete;
	inline EntityEventID &operator=(const EntityEventID &&other) noexcept = delete;

	// Member variables
	const UUID id;
	const std::string_view name;
};

}

// Define a hash functor for EntityEventID
template <>
struct std::hash<RassEngine::Events::EntityEventID> {
	std::size_t operator()(const RassEngine::Events::EntityEventID &event) const noexcept {
		return hash<RassEngine::UUID>()(event.GetID());
	}
};
