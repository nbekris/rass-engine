//------------------------------------------------------------------------------
//
// File Name:	ParticleManager.cpp
// Author(s):	taro.omiya
// Course:		CS529F25
// Project:		Project 7
// Purpose:		This component class is responsible for managing particles.
//
// Copyright © 2025 DigiPen (USA) Corporation.
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Includes:
//------------------------------------------------------------------------------

#include "Precompiled.h"
#include "ParticleManager.h"
#include "Component.h"
#include "Entity.h"
#include "Stream.h"
#include "Utils.h"
#include "MeshLibrary.h"
#include "SpriteSourceLibrary.h"
#include "Mesh.h"
#include "SpriteSource.h"

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
	
	const char* ParticleManager::KEY = "Particles";
	const char* ParticleManager::NAME = "ParticleManager";
	static const char* PARTICLE_MAX = "ParticleMax";
	static const char* IS_LOOPING = "IsLooping";

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

	ParticleManager::ParticleManager(void)
		: Component()
	{
	}

	ParticleManager::ParticleManager(const ParticleManager* other)
		: Component(other)
		, maxParticles(other->maxParticles)
		, areRecyclable(other->areRecyclable)
		, mesh(other->mesh)
		, spriteSource(other->spriteSource)
	{ }

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

	bool ParticleManager::Initialize()
	{
		// Reset everything
		particles.clear();
		particleActive = 0;
		particleFree = maxParticles;
		particleAlive = maxParticles;

		// Allocate the maximum number of particles to avoid resizing.
		particles.reserve(particleFree);
		while (particles.size() < particleFree)
		{
			particles.emplace_back();
		}
		return true;
	}

	void ParticleManager::Update(float dt)
	{
		//----------------------------------------------------------------------------
		// Update all existing particles.
		//----------------------------------------------------------------------------

		// For each active particle
		unsigned i = 0;
		while((i < particles.size()) && particles[i].current.IsActive())
		{
			// Decrement the lifetime by dt.
			particles[i].current.lifetime -= dt;
			if (particles[i].current.IsActive())
			{
				// Since KillParticle() performs a swap,
				// increment the index when it's *not* called
				// in this loop
				++i;
			}
			else
			{
				// DEAD PARTICLE:
				// Kill the particle.
				KillParticle(i);
			}
		}

		//----------------------------------------------------------------------------
		// Clean up.
		//----------------------------------------------------------------------------

		// If the container is both "full" and "empty",
		// [HINT: This means that there are no active or free particles remaining.]
		if (IsEmpty() && IsFull())
		{
			// Get the emitter's parent entity.
			Entity* parent = Parent();
			if (parent != nullptr)
			{
				// Mark the entity for destruction (Entity::Destroy).
				parent->Destroy();
			}
		}
	}

	void ParticleManager::Render() const
	{
		// Check if sprite is valid
		if (spriteSource != nullptr)
		{
			// Set shader to texture mode
			DGL_Graphics_SetShaderMode(DGL_PixelShaderMode::DGL_PSM_TEXTURE, DGL_VertexShaderMode::DGL_VSM_DEFAULT);

			// Load the sprite
			spriteSource->SetTextureOffset(0);
			spriteSource->UseTexture();
		}
		else
		{
			// Set shader to color mode
			DGL_Graphics_SetShaderMode(DGL_PixelShaderMode::DGL_PSM_COLOR, DGL_VertexShaderMode::DGL_VSM_DEFAULT);
		}

		// Render each active particle.
		for (const ParticleData& data : particles)
		{
			// Check for an active particle
			if (!data.current.IsActive())
			{
				// if a particle isn't active, halt immediately
				break;
			}

			// Pass the alpha value (1.0f) to the DGL.
			DGL_Graphics_SetCB_Alpha(data.current.color.a);

			// Pass the tint color to the DGL.
			DGL_Graphics_SetCB_TintColor(&data.current.color);

			// Send the transform data to the DGL.
			DGL_Graphics_SetCB_TransformData(&data.current.position, &data.current.scale, data.current.rotationRad);

			// Render the mesh associated with the emitter.
			mesh->Render();
		}
	}

	void ParticleManager::Read(Stream& stream)
	{
		// Check for valid stream (optional).
		Utils::IsStreamVerified(stream, KEY, NAME);

		// Traverse down the tree to the "ParticleEmitter" object (PushNode).
		stream.PushNode(KEY);

		Utils::ReadOptionalAttribute(stream, PARTICLE_MAX, maxParticles, NAME);
		Utils::ReadOptionalAttribute(stream, IS_LOOPING, areRecyclable, NAME);

		// Read the name of the mesh.
		std::string fileName;
		if (stream.Read("Mesh", fileName))
		{
			// Build the requested mesh.
			mesh = MeshLibrary::Build(fileName);
		}

		// Read the name of the sprite.
		if (stream.Read("SpriteSource", fileName))
		{
			// Build the requested sprite.
			spriteSource = SpriteSourceLibrary::Build(fileName);
		}

		// Return to the original location in the tree.
		stream.PopNode();
	}

	std::tuple<ParticleManager::StartingStats*, Particle*> ParticleManager::AllocateParticle()
	{
		// If the container is NOT full (no free particles).
		if (IsFull())
		{
			return { nullptr, nullptr };
		}

		// Jump to the first active particle
		ParticleData& data = particles[particleActive];

		// Increment the number of active particles.
		++particleActive;

		// Decrement the number of free particles.
		--particleFree;

		// Return a reference to the current particle.
		return { &data.starting, &data.current };
	}

	void ParticleManager::ForEachActiveParticle(particleUpdater lambda)
	{
		for (ParticleData& data : particles)
		{
			// Check for an active particle
			if (!data.current.IsActive())
			{
				// If this particle isn't active, halt immediately
				break;
			}

			// Apply changes
			lambda(data.starting, data.current);
		}
	}

#pragma endregion Public Functions

	//--------------------------------------------------------------------------
	// Private Functions:
	//--------------------------------------------------------------------------

#pragma region Private Functions

	void ParticleManager::KillParticle(unsigned particleIndex)
	{
		DRAGON_ASSERT(particleActive > 0, "ParticleManager::KillParticle: no active particles to kill");
		DRAGON_ASSERT(particleIndex < particleActive, "ParticleManager::KillParticle: attemtping to kill an inactive particle index");
		DRAGON_ASSERT(particleIndex < particles.size(), "ParticleManager::KillParticle: attemtping to kill a non-existent particle index");

		const unsigned indexLastActiveParticle = particleActive - 1;

		// If the particles are "recyclable".
		if (areRecyclable)
		{
			// Set the particle's lifetime variable to zero.
			particles[particleIndex].current.lifetime = 0.f;

			// Increment the number of free particles.
			++particleFree;

			// Check if the particle *isn't* the last active one in the list
			if (particleIndex < indexLastActiveParticle)
			{
				// When a particle dies, swap it's position:
				// If recyclable, swap with the last active particle
				std::swap(particles[particleIndex], particles[indexLastActiveParticle]);
			}
		}
		else
		{
			// Set the particle's lifetime variable to a negative value.
			particles[particleIndex].current.lifetime = -1.f;

			// Check if the particle *isn't* the last active one in the list
			const unsigned indexLastLivingParticle = particleAlive - 1;
			if (particleIndex < indexLastLivingParticle)
			{
				// When a particle dies, swap it's position:
				// Start by swapping near the end where all the dead particles are
				std::swap(particles[particleIndex], particles[indexLastLivingParticle]);

				// Check if the particle is *not* active
				if (!particles[particleIndex].current.IsActive() && (particleIndex < indexLastActiveParticle))
				{
					// Swap the inactive particle, with the last active particle
					std::swap(particles[particleIndex], particles[indexLastActiveParticle]);
				}
			}

			// Decrement the last undead particle
			--particleAlive;
		}

		// Decrement the number of active particles.
		--particleActive;
	}

#pragma endregion Private Functions

}	// namespace