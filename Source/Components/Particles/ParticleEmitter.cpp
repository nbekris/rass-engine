//------------------------------------------------------------------------------
//
// File Name:	ParticleEmitter.cpp
// Author(s):	dschilling
// Course:		CS529F25
// Project:		Project X
// Purpose:		This component class is responsible for emitting and managing particles.
//
// Copyright © 2025 DigiPen (USA) Corporation.
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Includes:
//------------------------------------------------------------------------------

#include "Precompiled.h"
#include <numbers>
#include <string>
#include <vector>

#include "DGL.h"
#include <dragon/assert.h>

#include "ParticleEmitter.h"
#include "Color.h"
#include "Component.h"
#include "Entity.h"
#include "Particle.h"
#include "ParticleManager.h"
#include "Random.h"
#include "Stream.h"
#include "Transform.h"
#include "Vector2D.h"
#include "ObjectFactory.h"
#include "Utils.h"
#include "LoggingSystem.h"
#include "TweenCurve.h"

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

//--------------------------------------------------------------------------
// Public Variables:
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Private Static Constants:
//--------------------------------------------------------------------------

static const char *PARTICLE_EMITTER = "ParticleEmitter";
static const char *LOG_TRANSFORM = "transform";
static const char *LOG_MANAGER = "ParticleManager";

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

ParticleEmitter::ParticleEmitter(void)
	: Component() {}

ParticleEmitter::ParticleEmitter(const ParticleEmitter *other)
	: Component(other)
	, emitRate(other->emitRate)
	, lifetimeMin(other->lifetimeMin)
	, lifetimeMax(other->lifetimeMax)
	, scaleMin(other->scaleMin)
	, scaleMax(other->scaleMax)
	, tintColor(other->tintColor)
	, isEmitting(other->isEmitting)
	, shape(other->shape) {}

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

bool ParticleEmitter::Initialize() {
	return true;
}

void ParticleEmitter::Update(float dt) {
	// Grab manager component
	ParticleManager *manager = Utils::GetComponentSafe<ParticleManager>(Parent(), LOG_MANAGER, PARTICLE_EMITTER);
	if(manager == nullptr) {
		return;
	}

	//----------------------------------------------------------------------------
	// Spawn new particles.
	//----------------------------------------------------------------------------

	if(isEmitting) {
		// Update the emit accumulator (accumulator += emitRate * dt).
		emitAccumulator += emitRate * dt;

		// While there are new particles to spawn.
		while(emitAccumulator > 0) {
			// If the container is not full,
			if(!manager->IsFull()) {
				// Spawn a new particle.
				auto [startingStats, particle] = manager->AllocateParticle();
				SetupParticle(startingStats, particle);
			}

			// Decrement the accumulator.
			--emitAccumulator;
		}
	}

	//----------------------------------------------------------------------------
	// Clean up.
	//----------------------------------------------------------------------------

	// If the container is both "full" and "empty",
	// [HINT: This means that there are no active or free particles remaining.]
	if(manager->IsEmpty() && manager->IsFull()) {
		// Get the emitter's parent entity.
		Entity *parent = Parent();
		if(parent != nullptr) {
			// Mark the entity for destruction (Entity::Destroy).
			Parent()->Destroy();
		}
	}
}

void ParticleEmitter::Read(Stream &stream) {
	// Check for valid stream (optional).
	DRAGON_ASSERT(stream.Contains(PARTICLE_EMITTER), "ParticleEmitter::Read: Failed to find required key: ParticleEmitter");

	// Traverse down the tree to the "ParticleEmitter" object (PushNode).
	stream.PushNode(PARTICLE_EMITTER);

	// Read the emitter properties, from "emitRate" to "scale".
	Utils::ReadOptionalAttribute(stream, "EmitOnStart", isEmitting, PARTICLE_EMITTER);
	Utils::ReadOptionalAttribute(stream, "EmitRate", emitRate, PARTICLE_EMITTER);
	Utils::ReadOptionalAttribute(stream, "LifeTimeMin", lifetimeMin, PARTICLE_EMITTER);
	Utils::ReadOptionalAttribute(stream, "LifeTimeMax", lifetimeMax, PARTICLE_EMITTER);
	Utils::ReadOptionalVector2D(stream, "ScaleMin", scaleMin, PARTICLE_EMITTER);
	Utils::ReadOptionalVector2D(stream, "ScaleMax", scaleMax, PARTICLE_EMITTER);

	// Read the tintColor.
	std::vector colorArgs = {1.f, 1.f, 1.f, 1.f};
	if(stream.Read("TintColor", colorArgs) && (colorArgs.size() >= 4)) {
		tintColor.r = colorArgs[0];
		tintColor.g = colorArgs[1];
		tintColor.b = colorArgs[2];
		tintColor.a = colorArgs[3];
	}

	stream.ReadObject("Shape", [this, &stream] (const std::string &key) {
		// Construct the object from the factory, first
		Object *toConvert = ObjectFactory::GetInstance().Create(key);
		if(toConvert == nullptr) {
			LoggingSystem::Debug("{}: unable to create object, {}", PARTICLE_EMITTER, key);
			return;
		}

		// See if this can be converted into a component
		shape = dynamic_cast<Particles::EmitterShape *>(toConvert);
		if(shape == nullptr) {
			// Clean up the object
			delete toConvert;

			// Log that object wasn't a component
			LoggingSystem::Debug("{}: created object, \"{}\", was not a component", PARTICLE_EMITTER, key);
			return;
		}

		// If so, read from the stream
		shape->Read(stream);
		});

	// Return to the original location in the tree.
	stream.PopNode();
}

void ParticleEmitter::SetupParticle(ParticleManager::StartingStats *startingStats, Particle *particle) {
	// If a particle was successfully allocated,
	if((startingStats == nullptr) || (particle == nullptr)) {
		return;
	}

	// Initialize the lifetime to a random value between lifetimeMin and lifetimeMax.
	startingStats->lifetime = Random::Range(lifetimeMin, lifetimeMax);
	particle->lifetime = startingStats->lifetime;

	// Setup random color
	startingStats->color = Color(Random::Range(0.f, tintColor.r), Random::Range(0.f, tintColor.g), Random::Range(0.f, tintColor.b), tintColor.a);
	particle->color = startingStats->color;

	// Set the particle's scale
	startingStats->scale = TweenCurve::Calculate(TweenCurve::Type::Linear, scaleMin, scaleMax, Random::Range(0.f, 1.f));
	particle->scale = startingStats->scale;

	// Confirm we have a shape
	if(shape == nullptr) {
		return;
	}

	// Get the parent entity's Transform.
	const Transform *transform = Utils::GetComponentSafe<Transform>(Parent(), LOG_TRANSFORM, PARTICLE_EMITTER);
	if(transform == nullptr) {
		return;
	}

	// Initialize the particle's position and rotationRad
	std::tie(particle->position, particle->rotationRad) = shape->GetEmitTransform(transform);

	// Compute the particle's velocities
	std::tie(startingStats->velocity, startingStats->angularVelocityRad) = shape->GetInitVelocities(*particle);
}

#pragma endregion Private Functions

}	// namespace
