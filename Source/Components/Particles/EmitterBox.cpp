#include "Precompiled.h"
#include "EmitterBox.h"

#include <glm/vec3.hpp>
#include <tuple>

#include "Components/Transform.h"
#include "EmitterShape.h"
#include "Particle.h"
#include "Random.h"
#include "Stream.h"
#include "Systems/Logging/ILoggingSystem.h"
#include "Utils.h"
#include "Graphics/Math.h"

namespace RassEngine::Components::Particles {

static const char *KEY_WIDTH = "Width";
static const char *KEY_HEIGHT = "Height";
static const char *KEY_RANDOMIZE_ROTATION = "RandomizeRotation";
static const char *TURN_SPEED_MIN = "TurnSpeedMin";
static const char *TURN_SPEED_MAX = "TurnSpeedMax";

EmitterBox::EmitterBox(void)
	: EmitterShape() {}

EmitterBox::EmitterBox(const EmitterBox &other)
	: EmitterShape(other)
	, width(other.width)
	, height(other.height)
	, isRotationRandom(other.isRotationRandom)
	, turnSpeedMinDeg(other.turnSpeedMinDeg)
	, turnSpeedMaxDeg(other.turnSpeedMaxDeg) {}

bool EmitterBox::Read(Stream &stream) {
	// Make sure stream is valid
	if(!EmitterShape::Read(stream)) {
		return false;
	}

	// Read the node values
	stream.PushNode(NAMEOF(EmitterBox));

	// Read the box width
	stream.Read(KEY_WIDTH, width);

	// Read the box height
	stream.Read(KEY_HEIGHT, height);

	// Read whether to randomize the particle rotation or not
	stream.Read(KEY_RANDOMIZE_ROTATION, isRotationRandom);

	// Read the velocities
	stream.Read(TURN_SPEED_MIN, turnSpeedMinDeg);
	stream.Read(TURN_SPEED_MAX, turnSpeedMaxDeg);

	stream.PopNode();
	return true;
}

std::tuple<glm::vec3, float> EmitterBox::GetEmitTransform(const Transform &transform) const {
	// Initialize the particle's position to the emitter's position (from parent's transform).
	glm::vec3 returnPosition = transform.GetPosition();
	returnPosition.x += Random::range(-width, width) / 2.f;
	returnPosition.y += Random::range(-height, height) / 2.f;

	// Initialize the particle's rotation to zero.
	float returnRotation = isRotationRandom ? Random::angleRad<float>() : transform.GetRotationRad();
	return {returnPosition, returnRotation};
}

std::tuple<glm::vec3, float> EmitterBox::GetInitVelocities(const Particle &particle) const {
	// Pick a random direction between 0 and 2*M_PI radians.
	// Calculate the particle's velocity by multiplying the direction by the speed.
	glm::vec3 returnVelocity = Graphics::Math::FromAngleRad(Random::angleRad<float>()) * GetRandomSpeed();

	// Calculate a random turn velocity
	float returnTurnVelocityDeg = Random::range(turnSpeedMinDeg, turnSpeedMaxDeg);
	return {returnVelocity, returnTurnVelocityDeg * Utils::DEG_TO_RAD};
}

}	// namespace
