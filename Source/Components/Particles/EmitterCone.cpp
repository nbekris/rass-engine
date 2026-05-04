#include "Precompiled.h"
#include "EmitterCone.h"

#include <glm/vec3.hpp>
#include <limits>
#include <tuple>

#include "Cloneable.h"
#include "Component.h"
#include "Components/Transform.h"
#include "EmitterShape.h"
#include "Particle.h"
#include "Random.h"
#include "Stream.h"
#include "Systems/Logging/ILoggingSystem.h"
#include "Utils.h"
#include "Graphics/Math.h"

namespace RassEngine::Components::Particles {

static const char *KEY_START_ANGLE = "StartAngle";
static const char *KEY_END_ANGLE = "EndAngle";
static const char *KEY_MIN_RADIUS = "MinRadius";
static const char *KEY_MAX_RADIUS = "MaxRadius";

EmitterCone::EmitterCone(void)
	: EmitterShape() {}

EmitterCone::EmitterCone(const EmitterCone& other)
	: EmitterShape(other)
	, startAngleDeg(other.startAngleDeg), endAngleDeg(other.endAngleDeg)
	, minRadius(other.minRadius), maxRadius(other.maxRadius) {}

const std::string_view &EmitterCone::NameClass() const {
	static constexpr std::string_view className = NAMEOF(RassEngine::Components::Particles::EmitterCone);
	return className;
}

bool EmitterCone::Read(Stream &stream) {
	// Make sure stream is valid
	if(!EmitterShape::Read(stream)) {
		return false;
	}

	// Read the node values
	stream.PushNode(NAMEOF(EmitterCone));

	// Read the cone's range
	stream.Read(KEY_START_ANGLE, startAngleDeg);
	stream.Read(KEY_END_ANGLE, endAngleDeg);
	LOG_ASSERT(!(startAngleDeg < 0.f || startAngleDeg > 360.f), "EmitterCone::Read: Start Angle not between 0 - 360 degrees");
	LOG_ASSERT(!(endAngleDeg < 0.f || endAngleDeg > 360.f), "EmitterCone::Read: End Angle not between 0 - 360 degrees");
	LOG_ASSERT((endAngleDeg - startAngleDeg) > -std::numeric_limits<float>::epsilon(), "EmitterCone::Read: endAngleDeg is less than startAngleDeg");

	// Read the cone's radiis
	stream.Read(KEY_MIN_RADIUS, minRadius);
	stream.Read(KEY_MAX_RADIUS, maxRadius);
	LOG_ASSERT((maxRadius - minRadius) > -std::numeric_limits<float>::epsilon(), "EmitterCone::Read: maxRadius is less than minRadius");

	stream.PopNode();
	return true;
}

std::tuple<glm::vec3, float> EmitterCone::GetEmitTransform(const Transform &transform) const {
	// Get a random rotation within a certain range
	float returnRotation = transform.GetRotationRad();
	returnRotation += Random::range((startAngleDeg * Utils::DEG_TO_RAD), (endAngleDeg * Utils::DEG_TO_RAD));

	// Get a random radius
	float radius = Random::range(minRadius, maxRadius);

	// Determin direction of offsetting the particle from the transform's position
	glm::vec3 offsetDir = Graphics::Math::FromAngleRad(returnRotation);

	// Convert above into a position
	glm::vec3 returnPosition = transform.GetPosition() + (offsetDir * radius);
	return {returnPosition, returnRotation};
}

std::tuple<glm::vec3, float> EmitterCone::GetInitVelocities(const Particle &particle) const {
	// Pick a random direction between 0 and 2*M_PI radians.
	glm::vec3 returnVelocity = Graphics::Math::FromAngleRad(particle.rotationRad);

	// Calculate the particle's velocity by multiplying the direction by the speed.
	returnVelocity *= GetRandomSpeed();
	return {returnVelocity, 0.f};
}

bool EmitterCone::Initialize() {
	return true;
}

std::unique_ptr<Component> EmitterCone::Clone() const {
	return std::make_unique<EmitterCone>(*this);
}

}	// namespace
