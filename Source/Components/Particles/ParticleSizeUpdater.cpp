//------------------------------------------------------------------------------
//
// File Name:	ParticleSizeUpdater.cpp
// Author(s):	taro.omiya
// Course:		CS529F25
// Project:		Project 7
// Purpose:		Updates the particle's size
//
// Copyright © 2025 DigiPen (USA) Corporation.
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Includes:
//------------------------------------------------------------------------------

#include "Precompiled.h"
#include "ParticleSizeUpdater.h"
#include "Component.h"
#include "Particle.h"
#include "ParticleEmitter.h"
#include "Utils.h"

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

const char *ParticleSizeUpdater::NAME = "ParticleSizeUpdater";
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

ParticleSizeUpdater::ParticleSizeUpdater(void)
	: Component() {}

ParticleSizeUpdater::ParticleSizeUpdater(const ParticleSizeUpdater *other)
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

void ParticleSizeUpdater::Update(float dt) {
	ParticleManager *manager = Utils::GetComponentSafe<ParticleManager>(Parent(), ParticleManager::NAME, NAME);
	if(manager == nullptr) {
		return;
	}

	// Adjust the scale for each particle
	manager->ForEachActiveParticle([this, dt] (const ParticleManager::StartingStats &startingStats, Particle &particle) {
		// Compute time
		float scalar = 1.f - particle.lifetime / startingStats.lifetime;
		scalar = curve.Calculate(scalar);

		// Update the particle's scale
		Vector2D newScale = startingStats.scale;
		newScale.Scale(scalar);
		particle.scale = newScale;
		});
}

void ParticleSizeUpdater::Read(Stream &stream) {
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
