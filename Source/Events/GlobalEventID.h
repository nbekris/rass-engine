// File Name:    GlobalEventID.h
// Author(s):    main Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Event ID definitions for global events.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <string_view>

#include "INamedEventID.h"

namespace RassEngine::Events {

class GlobalEventID : INamedEventID {
public:
	/// <summary>
	/// Constructs an event ID with name
	/// </summary>
	GlobalEventID(const std::string_view &name);
	inline virtual ~GlobalEventID() = default;

	operator UUID::Type() const override;
	const UUID &GetID() const override;
	const std::string_view &GetName() const override;

	bool operator==(const GlobalEventID &other) const;
	bool operator!=(const GlobalEventID &other) const;

private:
	// Delete some defaults
	GlobalEventID() = delete;
	GlobalEventID(const GlobalEventID &other) = delete;
	GlobalEventID(const GlobalEventID &&other) noexcept = delete;
	GlobalEventID &operator=(const GlobalEventID &other) = delete;
	inline GlobalEventID &operator=(const GlobalEventID &&other) noexcept = delete;

	// Member variables
	const UUID id;
	const std::string_view name;
};

}

// Define a hash functor for GlobalEventID
template <>
struct std::hash<RassEngine::Events::GlobalEventID> {
	std::size_t operator()(const RassEngine::Events::GlobalEventID &event) const noexcept {
		return hash<RassEngine::UUID>()(event.GetID());
	}
};
