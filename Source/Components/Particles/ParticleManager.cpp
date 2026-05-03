// File Name:    ParticleManager.cpp
// Author(s):    main Taro Omiya, secondary Steven Yacoub, Niko Bekris, Eric Fleegal
// Course:       GAM541
// Project:      RASS
// Purpose:      File stream utilities for reading and writing data.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "ParticleManager.h"

#include <string>
#include <string_view>
#include <tuple>
#include <utility>

#include "Cloneable.h"
#include "Component.h"
#include "Entity.h"
#include "Events/Global.h"
#include "Events/GlobalEventArgs.h"
#include "Graphics/Math.h"
#include "Graphics/Mesh.h"
#include "Graphics/Texture.h"
#include "IEvent.h"
#include "Particle.h"
#include "Stream.h"
#include "Systems/GlobalEvents/IGlobalEventsSystem.h"
#include "Systems/Logging/ILoggingSystem.h"
#include "Systems/Render/IRenderSystem.h"
#include "Systems/Resource/IResourceSystem.h"
#include "Systems/Time/ITimeSystem.h"
#include "Utils.h"

using namespace RassEngine::Systems;
using namespace RassEngine::Events;
using namespace RassEngine::Graphics;

namespace RassEngine::Components::Particles {

static const char *PARTICLE_MAX = "ParticleMax";
static const char *IS_LOOPING = "IsLooping";
static const char *MESH = "Mesh";
static const char *TEXTURE = "Texture";

ParticleManager::ParticleManager(void)
	: Cloneable<Component, ParticleManager>()
	, updateListener{this, &ParticleManager::Update}
	, renderListener{this, &ParticleManager::Render}
{}

ParticleManager::ParticleManager(const ParticleManager &other)
	: Cloneable<Component, ParticleManager>{other}
	, maxParticles{other.maxParticles}
	, areRecyclable{other.areRecyclable}
	, mesh{other.mesh}
	, texture{other.texture}
	, updateListener{this, &ParticleManager::Update}
	, renderListener{this, &ParticleManager::Render}
{}

ParticleManager::~ParticleManager() {
	if(IGlobalEventsSystem::Get() == nullptr) {
		return;
	}

	IGlobalEventsSystem::Get()->unbind(Global::Update, &updateListener);
	IGlobalEventsSystem::Get()->unbind(Global::Render, &renderListener);
}

bool ParticleManager::Initialize() {
	// Reset everything
	particles.clear();
	particleActive = 0;
	particleFree = maxParticles;
	particleAlive = maxParticles;

	// Allocate the maximum number of particles to avoid resizing.
	particles.reserve(particleFree);
	while(particles.size() < particleFree) {
		particles.emplace_back();
	}

	// Make sure all systems are available
	if(IGlobalEventsSystem::Get() == nullptr) {
		LOG_ERROR("Cannot run particles: {} is not registered", NAMEOF(Systems::IGlobalEventsSystem));
		return false;
	}
	if(ITimeSystem::Get() == nullptr) {
		LOG_ERROR("Cannot run particles: {} is not registered", NAMEOF(Systems::ITimeSystem));
		return false;
	}
	if(IRenderSystem::Get() == nullptr) {
		LOG_ERROR("Cannot run particles: {} is not registered", NAMEOF(Systems::IRenderSystem));
		return false;
	}

	// Bind to events
	IGlobalEventsSystem::Get()->bind(Global::Update, &updateListener);
	IGlobalEventsSystem::Get()->bind(Global::Render, &renderListener);
	return true;
}

bool ParticleManager::Update(const IEvent<GlobalEventArgs> *, const GlobalEventArgs &) {
	//----------------------------------------------------------------------------
	// Update all existing particles.
	//----------------------------------------------------------------------------
	float dt = ITimeSystem::Get()->GetDeltaTimeSec();

	// For each active particle
	unsigned i = 0;
	while((i < particles.size()) && particles[i].current.IsActive()) {
		// Decrement the lifetime by dt.
		particles[i].current.lifetime -= dt;
		if(particles[i].current.IsActive()) {
			// Since KillParticle() performs a swap,
			// increment the index when it's *not* called
			// in this loop
			++i;
		} else {
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
	if(IsEmpty() && IsFull()) {
		// Get the emitter's parent entity.
		Entity *parent = Parent();
		if(parent != nullptr) {
			// Mark the entity for destruction (Entity::Destroy).
			parent->Destroy();
		}
	}
	return true;
}

bool ParticleManager::Render(const IEvent<GlobalEventArgs> *, const GlobalEventArgs &) {
	// Render each active particle.
	for(const ParticleData &data : particles) {
		// Check for an active particle
		if(!data.current.IsActive()) {
			// if a particle isn't active, halt immediately
			break;
		}

		IRenderSystem::Renderable particle;
		particle.mesh = mesh;
		particle.texture = texture;

		// Pass the alpha value (1.0f) to the DGL.
		particle.alpha = data.current.color.a;

		// Pass the tint color to the DGL.
		particle.color = glm::vec3(data.current.color.r, data.current.color.g, data.current.color.b);

		// Send the transform data to the DGL.
		particle.modelMatrix = Graphics::Math::GetTransformMatrix(data.current.position, data.current.rotationRad, data.current.scale);

		// Render the mesh associated with the emitter.
		IRenderSystem::Get()->SubmitRenderable(particle);
	}
	return true;
}

bool ParticleManager::Read(Stream &stream) {
	if(!Component::Read(stream)) {
		return false;
	}

	// Traverse down the tree to the "ParticleEmitter" object (PushNode).
	stream.PushNode(NAMEOF(ParticleManager));

	stream.Read(PARTICLE_MAX, maxParticles);
	stream.Read(IS_LOOPING, areRecyclable);

	// Read the name of the mesh.
	std::string fileName;
	if(stream.Read(MESH, fileName)) {
		if(IResourceSystem::Get() == nullptr) {
			LOG_ERROR("Cannot run particles: {} is not registered", NAMEOF(Systems::IResourceSystem));
			return false;
		}

		// Build the requested mesh.
		mesh = IResourceSystem::Get()->GetCustomMesh(fileName);
	}

	// Read the name of the sprite.
	if(stream.Read(TEXTURE, fileName)) {
		if(IResourceSystem::Get() == nullptr) {
			LOG_ERROR("Cannot run particles: {} is not registered", NAMEOF(Systems::IResourceSystem));
			return false;
		}

		// Build the requested sprite.
		texture = IResourceSystem::Get()->GetTexture(fileName);
	}

	// Return to the original location in the tree.
	stream.PopNode();
	return true;
}

std::tuple<ParticleManager::StartingStats *, Particle *> ParticleManager::AllocateParticle() {
	// If the container is NOT full (no free particles).
	if(IsFull()) {
		return {nullptr, nullptr};
	}

	// Jump to the first active particle
	ParticleData &data = particles[particleActive];

	// Increment the number of active particles.
	++particleActive;

	// Decrement the number of free particles.
	--particleFree;

	// Return a reference to the current particle.
	return {&data.starting, &data.current};
}

void ParticleManager::ForEachActiveParticle(particleUpdater lambda) {
	for(ParticleData &data : particles) {
		// Check for an active particle
		if(!data.current.IsActive()) {
			// If this particle isn't active, halt immediately
			break;
		}

		// Apply changes
		lambda(data.starting, data.current);
	}
}

void ParticleManager::KillParticle(unsigned particleIndex) {
	LOG_ASSERT(particleActive > 0, "ParticleManager::KillParticle: no active particles to kill");
	LOG_ASSERT(particleIndex < particleActive, "ParticleManager::KillParticle: attemtping to kill an inactive particle index");
	LOG_ASSERT(particleIndex < particles.size(), "ParticleManager::KillParticle: attemtping to kill a non-existent particle index");

	const unsigned indexLastActiveParticle = particleActive - 1;

	// If the particles are "recyclable".
	if(areRecyclable) {
		// Set the particle's lifetime variable to zero.
		particles[particleIndex].current.lifetime = 0.f;

		// Increment the number of free particles.
		++particleFree;

		// Check if the particle *isn't* the last active one in the list
		if(particleIndex < indexLastActiveParticle) {
			// When a particle dies, swap it's position:
			// If recyclable, swap with the last active particle
			std::swap(particles[particleIndex], particles[indexLastActiveParticle]);
		}
	} else {
		// Set the particle's lifetime variable to a negative value.
		particles[particleIndex].current.lifetime = -1.f;

		// Check if the particle *isn't* the last active one in the list
		const unsigned indexLastLivingParticle = particleAlive - 1;
		if(particleIndex < indexLastLivingParticle) {
			// When a particle dies, swap it's position:
			// Start by swapping near the end where all the dead particles are
			std::swap(particles[particleIndex], particles[indexLastLivingParticle]);

			// Check if the particle is *not* active
			if(!particles[particleIndex].current.IsActive() && (particleIndex < indexLastActiveParticle)) {
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

const std::string_view &ParticleManager::NameClass() const {
	static constexpr std::string_view className = NAMEOF(RassEngine::Components::Particles::ParticleManager);
	return className;
}

}	// namespace
