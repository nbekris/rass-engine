//------------------------------------------------------------------------------
//
// File Name:	ParticleSystem.cpp
// Author(s):	dschilling
// Course:		CS529F25
// Project:		Project 2
// Purpose:		Template class for a new system.
//
// Copyright © 2025 DigiPen (USA) Corporation.
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Includes:
//------------------------------------------------------------------------------

#include "Precompiled.h"
#include <cassert>

#include "BehaviorCursor.h"		// This behavior is registered here for expediency.
#include "ParticleEmitter.h"
#include "ParticleSystem.h"
#include "ObjectFactory.h"
#include "System.h"
#include "EmitterBox.h"
#include "EmitterCone.h"
#include "ParticleMover.h"
#include "ParticleSizeUpdater.h"
#include "ParticleAlphaUpdater.h"
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

	//--------------------------------------------------------------------------
	// Public Variables:
	//--------------------------------------------------------------------------

	//--------------------------------------------------------------------------
	// Private Static Constants:
	//--------------------------------------------------------------------------

	//------------------------------------------------------------------------------
	// Private Static Variables:
	//------------------------------------------------------------------------------

	ParticleSystem* ParticleSystem::instance = nullptr;

	//--------------------------------------------------------------------------
	// Private Constants:
	//--------------------------------------------------------------------------

	//--------------------------------------------------------------------------
	// Private Variables:
	//--------------------------------------------------------------------------

	//--------------------------------------------------------------------------
	// Constructors/Destructors:
	//--------------------------------------------------------------------------

#pragma region Constructors

	ParticleSystem::ParticleSystem(void)
		: System("ParticleSystem")
	{
		// Raise an assert if this system has already been created.
		assert(instance == nullptr);

		// Store this system's instance for use by static functions.
		instance = this;
	}

	//--------------------------------------------------------------------------

	ParticleSystem::~ParticleSystem(void)
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

#pragma endregion Public Functions

	//--------------------------------------------------------------------------
	// Private Functions:
	//--------------------------------------------------------------------------

#pragma region Private Functions

	bool ParticleSystem::Initialize()
	{
		// Add system-specific initialization code here.

		// Register the "ParticleEmitter" and "BehaviorCursor" components.
		ObjectFactory::GetInstance().Register("ParticleEmitter", []() { return new ParticleEmitter(); });
		ObjectFactory::GetInstance().Register("BehaviorCursor", []() { return new BehaviorCursor(); });
		ObjectFactory::GetInstance().Register(ParticleMover::NAME, []() { return new ParticleMover(); });
		ObjectFactory::GetInstance().Register(ParticleSizeUpdater::NAME, []() { return new ParticleSizeUpdater(); });
		ObjectFactory::GetInstance().Register(ParticleAlphaUpdater::NAME, []() { return new ParticleAlphaUpdater(); });
		ObjectFactory::GetInstance().Register(ParticleManager::KEY, []() { return new ParticleManager(); });
		ObjectFactory::GetInstance().Register(Particles::EmitterBox::KEY, []() { return new Particles::EmitterBox(); });
		ObjectFactory::GetInstance().Register(Particles::EmitterCone::KEY, []() { return new Particles::EmitterCone(); });

		// Return true if the initialization completed successfully.
		return true;
	}

#pragma endregion Private Functions

}	// namespace