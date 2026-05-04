// File Name:    ParticleMover.cpp
// Author(s):    main Taro Omiya, secondary Steven Yacoub, Niko Bekris, Eric Fleegal
// Course:       GAM541
// Project:      RASS
// Purpose:      File stream utilities for reading and writing data.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "ParticleMover.h"

#include <glm/vec3.hpp>

#include "Component.h"
#include "Entity.h"
#include "Events/Global.h"
#include "ParticleManager.h"
#include "Stream.h"
#include "Systems/GlobalEvents/IGlobalEventsSystem.h"
#include "Systems/Logging/ILoggingSystem.h"
#include "Systems/Time/ITimeSystem.h"
#include "Utils.h"

using namespace RassEngine::Systems;
using namespace RassEngine::Events;
using namespace RassEngine::Graphics;

namespace RassEngine::Components::Particles {

#pragma region Constructors

ParticleMover::ParticleMover(void)
	: Cloneable<Component, ParticleMover>()
	, updateListener{this, &ParticleMover::Update}
{}

ParticleMover::ParticleMover(const ParticleMover &other)
	: Cloneable<Component, ParticleMover>(other)
	, updateListener{this, &ParticleMover::Update}
{}

ParticleMover::~ParticleMover(void) {
	if(IGlobalEventsSystem::Get() == nullptr) {
		return;
	}

	// Unbind from the event
	IGlobalEventsSystem::Get()->unbind(Global::Update, &updateListener);
}

#pragma endregion Constructors

//--------------------------------------------------------------------------
// Public Functions:
//--------------------------------------------------------------------------

#pragma region Public Functions

bool ParticleMover::Initialize() {
	// Retrieve the particle manager
	if(Parent() == nullptr) {
		LOG_ERROR("Cannot move particles: {} is not attached to an {}", NameClass(), NAMEOF(Entity));
		return false;
	}

	manager = Parent()->Get<ParticleManager>();
	if(manager == nullptr) {
		LOG_ERROR("Cannot move particles: {} is not attached to an {}", NAMEOF(ParticleManager), NAMEOF(Entity));
		return false;
	}

	// Make sure all systems are available
	if(IGlobalEventsSystem::Get() == nullptr) {
		LOG_ERROR("Cannot move particles: {} is not registered", NAMEOF(Systems::IGlobalEventsSystem));
		return false;
	}
	if(ITimeSystem::Get() == nullptr) {
		LOG_ERROR("Cannot move particles: {} is not registered", NAMEOF(Systems::ITimeSystem));
		return false;
	}

	// Bind to events
	IGlobalEventsSystem::Get()->bind(Global::Update, &updateListener);
	return true;
}

const std::string_view &ParticleMover::NameClass() const {
	static constexpr std::string_view className = NAMEOF(RassEngine::Components::Particles::ParticleMover);
	return className;
}

bool ParticleMover::Read(Stream &stream) {
	// Make sure stream is valid
	if(!Component::Read(stream)) {
		return false;
	}

	// Read the node values
	stream.PushNode(NAMEOF(ParticleMover));

	stream.PopNode();
	return true;
}

#pragma endregion Public Functions

//--------------------------------------------------------------------------
// Private Functions:
//--------------------------------------------------------------------------

#pragma region Private Functions

bool ParticleMover::Update(const IEvent<GlobalEventArgs> *, const GlobalEventArgs &) {
	float dt = ITimeSystem::Get()->GetDeltaTimeSec();

	// Adjust the transform for each particle
	manager->ForEachActiveParticle([this, dt] (const ParticleManager::StartingStats &startingStats, Particle &particle) {
		// Update the position of the active particle.
		particle.position += startingStats.velocity * dt;

		// Update the rotation of the active particle.
		particle.rotationRad += startingStats.angularVelocityRad * dt;
	});
	return true;
}

#pragma endregion Private Functions

}	// namespace
