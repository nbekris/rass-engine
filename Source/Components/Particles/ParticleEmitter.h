//------------------------------------------------------------------------------
//
// File Name:	ParticleEmitter.h
// Author(s):	dschilling
// Course:		CS529F25
// Project:		Project X
// Purpose:		This component class is responsible for emitting and managing particles.
//
// Copyright © 2025 DigiPen (USA) Corporation.
//
//------------------------------------------------------------------------------

#pragma once

//------------------------------------------------------------------------------
// Includes:
//------------------------------------------------------------------------------

#include "DGL.h"

#include "Component.h"
#include "Vector2D.h"
#include "ParticleManager.h"
#include "EmitterShape.h"

//------------------------------------------------------------------------------
// External Declarations:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Namespace Declarations:
//------------------------------------------------------------------------------

// Forward Declarations:
class Mesh;
class SpriteSource;
class Stream;

namespace RassEngine::Components::Particles {

// Typedefs:

// Class Definition:
class ParticleEmitter : public Component {
	// Public Constants and Enums:
public:

	// Constructors/Destructors:
public:
	ParticleEmitter(void);

	// @brief This copy-constructor should perform a shallow copy of the data.
	ParticleEmitter(const ParticleEmitter *other);

	virtual ~ParticleEmitter(void) override {};

	// Public Static Functions:
public:

	// Public Functions:
public:
	inline bool IsEmitting() const {
		return isEmitting;
	}
	inline void IsEmitting(bool setToEmit) {
		isEmitting = setToEmit;
	}

	// Public Event Handlers
public:

	// Private Functions:
private:
	// @brief This function is required to invoke the copy-constructor in derived classes.
	ParticleEmitter *Clone() const override {
		return new ParticleEmitter(this);
	}

	// @brief Initialize the component.
	// @brief [NOTE: Called when a new entity is initialized after creation.]
	// @brief [NOTE: Many components won't require this step; others might.]  
	//
	// @return bool = true if initialization successful, otherwise false.
	bool Initialize() override;

	// @brief Update the component each frame.
	//
	// @param dt = Delta time (in seconds) of the last frame.
	void Update(float dt) override;

	// @brief Render the component each frame.
	// @brief [NOTE: Modern engines handle rendering in a more complicated way.]
	// @brief [NOTE: Some components are rendered only when debug drawing is enabled.]
	void Render() const override {};

	// @brief Read the properties of a ParticleEmitter component from a stream.
	// @brief Specific Steps:
	// @brief   Check for valid stream (optional).
	// @brief   Traverse down the tree to the "ParticleEmitter" object (PushNode).
	// @brief   Read component-specific data here.
	// @brief   Return to the original location in the tree (PopObject).
	//
	// @param stream = The data stream used for reading.
	void Read(Stream &stream);

	void SetupParticle(ParticleManager::StartingStats *startingStats, Particle *particle);

	// Private Constants:
private:

	// Private Static Variables:
private:

	// Private Variables:
private:
	// Tracks the number of particles to emit, including "partial" particles.
	// This approach works with both large and fractional numbers.
	float emitAccumulator{0.0f};

	bool isEmitting{true};
	float emitRate{1.0f};
	float lifetimeMin{1.0f};
	float lifetimeMax{1.0f};
	Vector2D scaleMin{1.0f, 1.0f};
	Vector2D scaleMax{1.0f, 1.0f};
	DGL_Color tintColor{1.0f, 1.0f, 1.0f, 1.0f};
	Particles::EmitterShape *shape{nullptr};
};

}	// namespace
