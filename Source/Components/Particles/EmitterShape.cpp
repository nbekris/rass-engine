//------------------------------------------------------------------------------
//
// File Name:	EmitterShape.cpp
// Author(s):	taro.omiya
// Course:		CS529F25
// Project:		Project 7
// Purpose:		Abstract class for particle emitter's shape of emittance.
//
// Copyright © 2025 DigiPen (USA) Corporation.
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Includes:
//------------------------------------------------------------------------------

#include "Precompiled.h"
#include "EmitterShape.h"
#include "Utils.h"

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

	//--------------------------------------------------------------------------
	// Public Variables:
	//--------------------------------------------------------------------------

	//--------------------------------------------------------------------------
	// Private Static Constants:
	//--------------------------------------------------------------------------

	const char* EMITTER_SHAPE = "EmitterShape";
	const char* SPEED_MIN = "SpeedMin";
	const char* SPEED_MAX = "SpeedMax";

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

	EmitterShape::EmitterShape(void)
		: Object()
	{
	}

	EmitterShape::EmitterShape(const EmitterShape* other)
		: Object(other)
		, speedMin(other->speedMin)
		, speedMax(other->speedMax)
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

	void EmitterShape::Read(CS529::Stream& stream)
	{
		Utils::ReadOptionalAttribute(stream, SPEED_MIN, speedMin, EMITTER_SHAPE);
		Utils::ReadOptionalAttribute(stream, SPEED_MAX, speedMax, EMITTER_SHAPE);
	}

#pragma endregion Public Functions

	//--------------------------------------------------------------------------
	// Private Functions:
	//--------------------------------------------------------------------------

#pragma region Private Functions

#pragma endregion Private Functions

}	// namespace