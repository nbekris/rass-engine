//------------------------------------------------------------------------------
//
// File Name:	ParticleAlphaUpdater.h
// Author(s):	taro.omiya
// Course:		CS529F25
// Project:		Project 7
// Purpose:		Updates the particle's alpha (opacity)
//
// Copyright © 2025 DigiPen (USA) Corporation.
//
//------------------------------------------------------------------------------

#pragma once

//------------------------------------------------------------------------------
// Includes:
//------------------------------------------------------------------------------

#include "Component.h"
#include "TweenCurve.h"

//------------------------------------------------------------------------------
// External Declarations:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Namespace Declarations:
//------------------------------------------------------------------------------

namespace RassEngine::Components::Particles {
// Forward Declarations:
class Stream;
class Particle;

// Typedefs:

// Class Definition:
class ParticleAlphaUpdater : public Component {
	// Public Constants and Enums:
public:
	static const char *NAME;

	// Constructors/Destructors:
public:
	ParticleAlphaUpdater(void);

	// @brief This copy-constructor should perform a shallow copy of the data.
	ParticleAlphaUpdater(const ParticleAlphaUpdater *other);

	~ParticleAlphaUpdater(void) override {};

	// Public Static Functions:
public:

	// Public Functions:
public:
	ParticleAlphaUpdater *Clone() const override {
		return new ParticleAlphaUpdater(this);
	}

	// @brief Initialize the component.
	// @brief [NOTE: Called when a new entity is initialized after creation.]
	// @brief [NOTE: Many components won't require this step; others might.]  
	//
	// @return bool = true if initialization successful, otherwise false.
	bool Initialize() override {
		return true;
	};

	// @brief Update the component each frame.
	//
	// @param dt = Delta time (in seconds) of the last frame.
	void Update(float dt) override;

	// @brief Render the component each frame.
	// @brief [NOTE: Modern engines handle rendering in a more complicated way.]
	// @brief [NOTE: Some components are rendered only when debug drawing is enabled.]
	void Render() const override {};

	// @brief Read the properties of a ParticleAlphaUpdater component from a stream.
	// @brief Specific Steps:
	// @brief   Check for valid stream (optional).
	// @brief   Traverse down the tree to the "ParticleAlphaUpdater" object (PushNode).
	// @brief   Read component-specific data here.
	// @brief   Return to the original location in the tree (PopObject).
	//
	// @param stream = The data stream used for reading.
	void Read(Stream &stream) override;

	// Public Event Handlers
public:

	// Private Functions:
private:

	// Private Constants:
private:

	// Private Static Variables:
private:

	// Private Variables:
private:
	TweenCurve curve{};
};

}	// namespace
