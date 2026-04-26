//------------------------------------------------------------------------------
//
// File Name:	ParticleManager.h
// Author(s):	taro.omiya
// Course:		CS529F25
// Project:		Project 7
// Purpose:		This component class is responsible for managing particles.
//
// Copyright © 2025 DigiPen (USA) Corporation.
//
//------------------------------------------------------------------------------

#pragma once

//------------------------------------------------------------------------------
// Includes:
//------------------------------------------------------------------------------

#include <functional>
#include <vector>
#include <tuple>

#include "Component.h"
#include "Particle.h"

//------------------------------------------------------------------------------
// External Declarations:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Namespace Declarations:
//------------------------------------------------------------------------------

namespace RassEngine::Components::Particles {
// Forward Declarations:
class Stream;
class Mesh;
class SpriteSource;

// Typedefs:

// Class Definition:
class ParticleManager : public Component {
	// Public Constants and Enums:
public:
	static const char *KEY;
	static const char *NAME;

	// Starting stats
	typedef struct StartingStats {
		// The duration this particle is intended to remain alive
		float lifetime{0};

		// The "physics" value(s).
		Vector2D velocity{};
		float angularVelocityRad{0.f};

		// The other starting stats
		Color color = Color::White;
		Vector2D scale = Vector2D{1.f, 1.f};
	} StartingStats;

	typedef std::function<void(const StartingStats &startingStats, Particle &particle)> particleUpdater;

	// Constructors/Destructors:
public:
	ParticleManager(void);

	// @brief This copy-constructor should perform a shallow copy of the data.
	ParticleManager(const ParticleManager *other);

	virtual ~ParticleManager(void) override {};

	// Public Static Functions:
public:

	// Public Functions:
public:
	ParticleManager *Clone() const {
		return new ParticleManager(this);
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
	void Render() const override;

	// @brief Read the properties of a ParticleManager component from a stream.
	// @brief Specific Steps:
	// @brief   Check for valid stream (optional).
	// @brief   Traverse down the tree to the "ParticleManager" object (PushNode).
	// @brief   Read component-specific data here.
	// @brief   Return to the original location in the tree (PopObject).
	//
	// @param stream = The data stream used for reading.
	void Read(Stream &stream) override;

	// Return true if particleActive == 0.
	bool IsEmpty() const {
		return particleActive == 0;
	}

	// Return true if particleFree == 0.
	bool IsFull() const {
		return particleFree == 0;
	}

	// Allocate an unused particle, if possible.
	std::tuple<StartingStats *, Particle *> AllocateParticle();

	void ForEachActiveParticle(particleUpdater lambda);

	// Public Event Handlers
public:
	typedef struct ParticleData {
		StartingStats starting{};
		Particle current{};
	} Data;

	// Private Functions:
private:

	// Kill an active particle.
	// [NOTE: When areRecyclable is true, particles become "Free", instead of "Dead".]
	void KillParticle(unsigned particleIndex);

	// Private Constants:
private:

	// Private Static Variables:
private:

	// Private Variables:
private:

	// A sorted container for the available particles:
	// Sorted with active particles first, followed by free, and finally, dead particles at the end
	// [NOTE: Allocate the maximum number of particles to avoid resizing.]
	std::vector<ParticleData> particles;

	// Indicates the max number of particles that can be allocated.
	unsigned maxParticles{100};

	// Indicates the number of active particles.
	unsigned particleActive{0};

	// Indicates the number of free/unused particles.
	unsigned particleFree{100};

	// Indicates which index to swap killed particles to.
	unsigned particleAlive{100};

	// Indicates that the particles can be recycled/reused after their lifetimer has expired.
	bool areRecyclable{false};

	const Mesh *mesh{nullptr};

	const SpriteSource *spriteSource{nullptr};
};

}	// namespace
