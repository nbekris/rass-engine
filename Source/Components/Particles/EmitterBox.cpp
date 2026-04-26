//------------------------------------------------------------------------------
//
// File Name:	EmitterBox.cpp
// Author(s):	taro.omiya
// Course:		CS529F25
// Project:		Project 7
// Purpose:		Behavior pattern for particle emission from a box.
//
// Copyright © 2025 DigiPen (USA) Corporation.
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Includes:
//------------------------------------------------------------------------------

#include "Precompiled.h"
#include "EmitterBox.h"
#include "Object.h"
#include "Stream.h"
#include "LoggingSystem.h"
#include "Utils.h"
#include "Random.h"

//------------------------------------------------------------------------------
// External Declarations:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Namespace Declarations:
//------------------------------------------------------------------------------

namespace RassEngine::Components::Particles {
//--------------------------------------------------------------------------
// Public Constants:
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Public Static Variables:
//--------------------------------------------------------------------------

const char *EmitterBox::KEY = "Box";
const char *EmitterBox::NAME = "ParticleEmitBox";
static const char *KEY_WIDTH = "Width";
static const char *KEY_HEIGHT = "Height";
static const char *KEY_RANDOMIZE_ROTATION = "RandomizeRotation";
static const char *TURN_SPEED_MIN = "TurnSpeedMin";
static const char *TURN_SPEED_MAX = "TurnSpeedMax";

//--------------------------------------------------------------------------
// Public Variables:
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Private Static Constants:
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Private Constants:
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Private Static Variables:
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Private Variables:
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Constructors/Destructors:
//--------------------------------------------------------------------------

#pragma region Constructors

EmitterBox::EmitterBox(void)
	: EmitterShape() {}

EmitterBox::EmitterBox(const EmitterBox *other)
	: EmitterShape(other)
	, width(other->width)
	, height(other->height)
	, isRotationRandom(other->isRotationRandom)
	, turnSpeedMinDeg(other->turnSpeedMinDeg)
	, turnSpeedMaxDeg(other->turnSpeedMaxDeg) {}

//--------------------------------------------------------------------------

EmitterBox::~EmitterBox(void) {}

#pragma endregion Constructors

//--------------------------------------------------------------------------
// Public Static Functions:
//--------------------------------------------------------------------------

#pragma region Public Static Functions

#pragma endregion Public Static Functions

	//--------------------------------------------------------------------------
	// Public Functions:
	//--------------------------------------------------------------------------

#pragma region Public Functions

void EmitterBox::Read(Stream &stream) {
	// Make sure stream is valid
	if(!Utils::IsStreamVerified(stream, KEY, NAME)) {
		return;
	}

	// Read the node values
	stream.PushNode(KEY);

	// Read the box width
	Utils::ReadOptionalAttribute(stream, KEY_WIDTH, width, NAME);

	// Read the box height
	Utils::ReadOptionalAttribute(stream, KEY_HEIGHT, height, NAME);

	// Read whether to randomize the particle rotation or not
	Utils::ReadOptionalAttribute(stream, KEY_RANDOMIZE_ROTATION, isRotationRandom, NAME);

	// Read the velocities
	EmitterShape::Read(stream);
	Utils::ReadOptionalAttribute(stream, TURN_SPEED_MIN, turnSpeedMinDeg, NAME);
	Utils::ReadOptionalAttribute(stream, TURN_SPEED_MAX, turnSpeedMaxDeg, NAME);

	stream.PopNode();
}

std::tuple<Vector2D, float> EmitterBox::GetEmitTransform(const Transform *transform) const {
	DRAGON_ASSERT(transform, "EmitterBox::GetEmitTransform: Transform is null");

	// Initialize the particle's position to the emitter's position (from parent's transform).
	Vector2D returnPosition = transform->Translation();
	returnPosition.x += Random::Range(-width, width) / 2.f;
	returnPosition.y += Random::Range(-height, height) / 2.f;

	// Initialize the particle's rotation to zero.
	float returnRotation = isRotationRandom ? Utils::RandomAngleRad() : transform->Rotation();
	return {returnPosition, returnRotation};
}

std::tuple<Vector2D, float> EmitterBox::GetInitVelocities(const Particle &particle) const {
	Vector2D returnVelocity;

	// Pick a random direction between 0 and 2*M_PI radians.
	returnVelocity.FromAngleRad(Utils::RandomAngleRad());

	// Calculate the particle's velocity by multiplying the direction by the speed.
	returnVelocity.Scale(GetRandomSpeed());

	// Calculate a random turn velocity
	float returnTurnVelocityDeg = Random::Range(turnSpeedMinDeg, turnSpeedMaxDeg);
	return {returnVelocity, returnTurnVelocityDeg * Utils::DEG_TO_RAD};

}
#pragma endregion Public Functions

//--------------------------------------------------------------------------
// Private Functions:
//--------------------------------------------------------------------------

#pragma region Private Functions

#pragma endregion Private Functions

}	// namespace
