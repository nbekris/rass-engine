//------------------------------------------------------------------------------
//
// File Name:	EmitterCone.cpp
// Author(s):	japta
// Course:		CS529F25
// Project:		Project 2
// Purpose:		Template class for a new object.
//
// Copyright © 2025 DigiPen (USA) Corporation.
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Includes:
//------------------------------------------------------------------------------

#include "Precompiled.h"
#include "EmitterCone.h"
#include <dragon/assert.h>
#include <numbers>
#include "Stream.h"
#include "Utils.h"
#include "Vector2D.h"

//------------------------------------------------------------------------------
// External Declarations:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Namespace Declarations:
//------------------------------------------------------------------------------

namespace CS529::Particles
{
	//--------------------------------------------------------------------------
	// Public Constants:
	//--------------------------------------------------------------------------

	//--------------------------------------------------------------------------
	// Public Static Variables:
	//--------------------------------------------------------------------------

	const char* EmitterCone::NAME = "ParticleEmitCone";
	const char* EmitterCone::KEY = "Cone";
	static const char* KEY_START_ANGLE = "StartAngle";
	static const char* KEY_END_ANGLE = "EndAngle";
	static const char* KEY_MIN_RADIUS = "MinRadius";
	static const char* KEY_MAX_RADIUS = "MaxRadius";

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

	EmitterCone::EmitterCone(void)
		: EmitterShape()
	{
	}

	EmitterCone::EmitterCone(const EmitterCone* other)
		: EmitterShape(other)
		, startAngleDeg(other->startAngleDeg), endAngleDeg(other->endAngleDeg)
		, minRadius(other->minRadius), maxRadius(other->maxRadius)
	{
	}

	//--------------------------------------------------------------------------

	EmitterCone::~EmitterCone(void)
	{
	}

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

	void EmitterCone::Read(Stream& stream)
	{
		// Make sure stream is valid
		if (!Utils::IsStreamVerified(stream, KEY, NAME))
		{
			return;
		}

		// Read the node values
		stream.PushNode(KEY);

		// Read the cone's range
		Utils::ReadOptionalAttribute(stream, KEY_START_ANGLE, startAngleDeg, NAME);
		Utils::ReadOptionalAttribute(stream, KEY_END_ANGLE, endAngleDeg, NAME);
		DRAGON_ASSERT(!(startAngleDeg < 0.f || startAngleDeg > 360.f), "EmitterCone::Read: Start Angle not between 0 - 360 degrees");
		DRAGON_ASSERT(!(endAngleDeg < 0.f || endAngleDeg > 360.f), "EmitterCone::Read: End Angle not between 0 - 360 degrees");
		DRAGON_ASSERT((endAngleDeg - startAngleDeg) > -std::numeric_limits<float>::epsilon(), "EmitterCone::Read: endAngleDeg is less than startAngleDeg");

		// Read the cone's radiis
		Utils::ReadOptionalAttribute(stream, KEY_MIN_RADIUS, minRadius, NAME);
		Utils::ReadOptionalAttribute(stream, KEY_MAX_RADIUS, maxRadius, NAME);
		DRAGON_ASSERT((maxRadius - minRadius) > -std::numeric_limits<float>::epsilon(), "EmitterCone::Read: maxRadius is less than minRadius");

		// Read the speed
		EmitterShape::Read(stream);

		stream.PopNode();
	}

	std::tuple<Vector2D, float> EmitterCone::GetEmitTransform(const Transform* transform) const
	{
		DRAGON_ASSERT(transform, "EmitterCone::GetEmitTransform: Transform is null");

		// Get a random rotation within a certain range
		float returnRotation = transform->Rotation();
		returnRotation += Random::Range((startAngleDeg * Utils::DEG_TO_RAD), (endAngleDeg * Utils::DEG_TO_RAD));

		// Get a random radius
		float radius = Random::Range(minRadius, maxRadius);

		// Determin direction of offsetting the particle from the transform's position
		Vector2D offsetDir;
		offsetDir.FromAngleRad(returnRotation);

		// Convert above into a position
		Vector2D returnPosition = transform->Translation();
		returnPosition.ScaleAdd(radius, offsetDir);
		return { returnPosition, returnRotation };
	}

	std::tuple<Vector2D, float> EmitterCone::GetInitVelocities(const Particle& particle) const
	{
		// Pick a random direction between 0 and 2*M_PI radians.
		Vector2D returnVelocity;
		returnVelocity.FromAngleRad(particle.rotationRad);

		// Calculate the particle's velocity by multiplying the direction by the speed.
		returnVelocity.Scale(GetRandomSpeed());
		return { returnVelocity, 0.f };
	}

#pragma endregion Public Functions

	//--------------------------------------------------------------------------
	// Private Functions:
	//--------------------------------------------------------------------------

#pragma region Private Functions

#pragma endregion Private Functions

}	// namespace