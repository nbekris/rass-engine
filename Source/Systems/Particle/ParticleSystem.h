//------------------------------------------------------------------------------
//
// File Name:	ParticleSystem.h
// Author(s):	dschilling
// Course:		CS529F25
// Project:		Project 2
// Purpose:		Template class for a new system.
//
// Copyright © 2025 DigiPen (USA) Corporation.
//
//------------------------------------------------------------------------------

#pragma once

//------------------------------------------------------------------------------
// Includes:
//------------------------------------------------------------------------------

#include "System.h"

//------------------------------------------------------------------------------
// External Declarations:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Namespace Declarations:
//------------------------------------------------------------------------------

namespace CS529
{
	// Forward Declarations:

	// Typedefs:

	// Class Definition:
	class ParticleSystem : public System
	{
		// Public Constants and Enums:
	public:

		// Constructors/Destructors:
	public:
		ParticleSystem(void);

		// All systems need a virtual destructor to have their destructor called 
		~ParticleSystem(void) override;

		// Public Static Functions:
	public:

		// Public Functions:
	public:

		// Public Event Handlers
	public:
		// Windows message event handler.
		//virtual bool HandleMessage(const Message& message) = 0;

		// Private Functions:
	private:
		// @brief Initialize the system.
		//
		// @return bool = true if initialization successful, otherwise false.
		bool Initialize() override;

		// Private Static Constants:
	private:

		// Private Static Variables:
	private:
		static ParticleSystem* instance;

		// Private Variables:
	private:
	};

}	// namespace