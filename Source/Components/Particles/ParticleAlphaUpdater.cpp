//------------------------------------------------------------------------------
//
// File Name:	ParticleAlphaUpdater.cpp
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
#include "ParticleAlphaUpdater.h"
#include "Component.h"
#include "Utils.h"
#include "Stream.h"
#include "ParticleManager.h"

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

const char *ParticleAlphaUpdater::NAME = "ParticleAlphaUpdater";
static const char *CURVE = "Curve";

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

ParticleAlphaUpdater::ParticleAlphaUpdater(void)
	: Component() {}

ParticleAlphaUpdater::ParticleAlphaUpdater(const ParticleAlphaUpdater *other)
	: Component(other), curve(other->curve) {}

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

void ParticleAlphaUpdater::Update(float dt) {
	ParticleManager *manager = Utils::GetComponentSafe<ParticleManager>(Parent(), ParticleManager::NAME, NAME);
	if(manager == nullptr) {
		return;
	}

	// Adjust the alpha for each particle
	manager->ForEachActiveParticle([this, dt] (const ParticleManager::StartingStats &startingStats, Particle &particle) {
		// Compute time
		float scalar = 1.f - particle.lifetime / startingStats.lifetime;
		scalar = curve.Calculate(scalar);

		// Update the particle's alpha
		particle.color.a = startingStats.color.a * scalar;
		});
}

void ParticleAlphaUpdater::Read(Stream &stream) {
	// Make sure stream is valid
	if(!Utils::IsStreamVerified(stream, NAME, NAME)) {
		return;
	}

	// Read the node values
	stream.PushNode(NAME);

	// Read the curve attributes
	curve.Read(stream);

	stream.PopNode();
}

#pragma endregion Public Functions

//--------------------------------------------------------------------------
// Private Functions:
//--------------------------------------------------------------------------

#pragma region Private Functions

#pragma endregion Private Functions

}	// namespace
