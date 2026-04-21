// File Name:    EntityEventID.cpp
// Author(s):    main Taro Omiya, secondary Eric Fleegal
// Course:       GAM541
// Project:      RASS
// Purpose:      Event ID definitions for entity-level events.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "Events/EntityEventID.h"

namespace RassEngine::Events {

EntityEventID::EntityEventID(const std::string_view &name) : id{}, name{name} {}

bool EntityEventID::operator==(const EntityEventID &other) const {
	return id == other.id;
}

bool EntityEventID::operator!=(const EntityEventID &other) const {
	return !operator==(other);
}

EntityEventID::operator UUID::Type() const {
	return static_cast<UUID::Type>(id);
}

const UUID &EntityEventID::GetID() const {
	return id;
}

const std::string_view &EntityEventID::GetName() const {
	return name;
}

}
