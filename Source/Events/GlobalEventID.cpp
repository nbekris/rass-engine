// File Name:    GlobalEventID.cpp
// Author(s):    main Taro Omiya, secondary Eric Fleegal
// Course:       GAM541
// Project:      RASS
// Purpose:      Event ID definitions for global events.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "Events/GlobalEventID.h"

namespace RassEngine::Events {

GlobalEventID::GlobalEventID(const std::string_view &name) : id{}, name{name} {}

bool GlobalEventID::operator==(const GlobalEventID &other) const {
	return id == other.id;
}

bool GlobalEventID::operator!=(const GlobalEventID &other) const {
	return !operator==(other);
}

GlobalEventID::operator UUID::Type() const {
	return static_cast<UUID::Type>(id);
}

const UUID &GlobalEventID::GetID() const {
	return id;
}

const std::string_view &GlobalEventID::GetName() const {
	return name;
}

}
