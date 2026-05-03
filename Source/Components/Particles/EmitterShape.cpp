// File Name:    EmitterShape.cpp
// Author(s):    main Taro Omiya, secondary Steven Yacoub, Niko Bekris, Eric Fleegal
// Course:       GAM541
// Project:      RASS
// Purpose:      File stream utilities for reading and writing data.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "EmitterShape.h"

#include "Cloneable.h"
#include "Component.h"
#include "Stream.h"

namespace RassEngine::Components::Particles {

const char *SPEED_MIN = "SpeedMin";
const char *SPEED_MAX = "SpeedMax";

EmitterShape::EmitterShape(void)
	: Cloneable<Component, EmitterShape>() {}

EmitterShape::EmitterShape(const EmitterShape &other)
	: Cloneable<Component, EmitterShape>(other)
	, speedMin(other.speedMin)
	, speedMax(other.speedMax) {}

bool EmitterShape::Read(Stream &stream) {
	// Make sure stream is valid
	if(!Component::Read(stream)) {
		return false;
	}

	stream.Read(SPEED_MIN, speedMin);
	stream.Read(SPEED_MAX, speedMax);
	return true;
}

}	// namespace
