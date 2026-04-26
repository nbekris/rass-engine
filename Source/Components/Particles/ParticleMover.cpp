//------------------------------------------------------------------------------
//
// File Name:	ParticleMover.cpp
// Author(s):	japta
// Course:		CS529F25
// Project:		Project 4
// Purpose:		This component class is responsible for ...
//
// Copyright © 2025 DigiPen (USA) Corporation.
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Includes:
//------------------------------------------------------------------------------

#include "Precompiled.h"
#include "ParticleMover.h"
#include "Component.h"
#include "Stream.h"
#include "Utils.h"
#include "ParticleManager.h"

//------------------------------------------------------------------------------
// External Declarations:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Namespace Declarations:
//------------------------------------------------------------------------------

namespace CS529
{
	//--------------------------------------------------------------------------
	// Public Constants:
	//--------------------------------------------------------------------------

	//--------------------------------------------------------------------------
	// Public Static Variables:
	//--------------------------------------------------------------------------

	const char* ParticleMover::NAME = "ParticleMover";

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

	ParticleMover::ParticleMover(void)
		: Component()
	{
	}

	ParticleMover::ParticleMover(const ParticleMover* other)
		: Component(other)
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

	void ParticleMover::Update(float dt)
	{
		ParticleManager* manager = Utils::GetComponentSafe<ParticleManager>(Parent(), ParticleManager::NAME, NAME);
		if (manager == nullptr)
		{
			return;
		}

		// Adjust the transform for each particle
		manager->ForEachActiveParticle([this, dt](const ParticleManager::StartingStats& startingStats, Particle& particle) {
			// Update the position of the active particle.
			particle.position.ScaleAdd(dt, startingStats.velocity);

			// Update the rotation of the active particle.
			particle.rotationRad += dt * startingStats.angularVelocityRad;
		});
	}

	void ParticleMover::Read(Stream& stream)
	{
		// Make sure stream is valid
		if (!Utils::IsStreamVerified(stream, NAME, NAME))
		{
			return;
		}

		// Read the node values
		stream.PushNode(NAME);

		stream.PopNode();
	}

#pragma endregion Public Functions

	//--------------------------------------------------------------------------
	// Private Functions:
	//--------------------------------------------------------------------------

#pragma region Private Functions

#pragma endregion Private Functions

}	// namespace