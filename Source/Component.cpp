// File Name:    Component.cpp
// Author(s):    main Niko Bekris, secondary Taro Omiya, Steven Yacoub, Eric Fleegal
// Course:       GAM541
// Project:      RASS
// Purpose:      Base component class for entity components.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "Component.h"

#include "Object.h"
#include "Stream.h"
#include "Systems/Logging/ILoggingSystem.h"

namespace RassEngine {

Component::Component(Component &&other) noexcept : Object{other} {
	std::swap(parent, other.parent);
}

bool Component::Read(Stream &stream) {
	// Make sure argument is valid before reading
	if(!stream.IsValid()) {
		LOG_WARNING("{}: Invalid stream argument", NameClass());
		return false;
	}

	// Read the enabled flag
	stream.Read("Enabled", isEnabled);
	return true;
}

}
