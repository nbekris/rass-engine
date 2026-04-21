// File Name:    INamedEventID.h
// Author(s):    main Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Interface for named event identifiers.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <string_view>

#include "UUID.h"

namespace RassEngine::Events {

class INamedEventID {
public:
	inline virtual ~INamedEventID() = default;

	virtual operator UUID::Type() const = 0;
	virtual const UUID &GetID() const = 0;
	virtual const std::string_view &GetName() const = 0;

protected:
	inline INamedEventID() = default;
private:
	// Delete some defaults
	INamedEventID(const INamedEventID &other) = delete;
	INamedEventID(const INamedEventID &&other) noexcept = delete;
	INamedEventID &operator=(const INamedEventID &other) = delete;
	inline INamedEventID &operator=(const INamedEventID &&other) noexcept = delete;
};

}
