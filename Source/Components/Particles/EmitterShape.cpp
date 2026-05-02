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
