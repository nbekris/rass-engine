// File Name:    ParticleSizeUpdater.cpp
// Author(s):    main Taro Omiya, secondary Steven Yacoub, Niko Bekris, Eric Fleegal
// Course:       GAM541
// Project:      RASS
// Purpose:      File stream utilities for reading and writing data.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "ParticleSizeUpdater.h"

#include <glm/vec3.hpp>
#include <string_view>

#include "Cloneable.h"
#include "Component.h"
#include "Entity.h"
#include "Events/Global.h"
#include "Events/GlobalEventArgs.h"
#include "IEvent.h"
#include "Particle.h"
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

ParticleSizeUpdater::ParticleSizeUpdater(void)
	: Cloneable<Component, ParticleSizeUpdater>()
	, updateListener{this, &ParticleSizeUpdater::Update}
{}

ParticleSizeUpdater::ParticleSizeUpdater(const ParticleSizeUpdater &other)
	: Cloneable<Component, ParticleSizeUpdater>(other)
	, updateListener{this, &ParticleSizeUpdater::Update}
{}

ParticleSizeUpdater::~ParticleSizeUpdater(void) {
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

bool ParticleSizeUpdater::Initialize() {
	// Retrieve the particle manager
	if(Parent() == nullptr) {
		LOG_ERROR("Cannot resize particles: {} is not attached to an {}", NameClass(), NAMEOF(Entity));
		return false;
	}

	manager = Parent()->Get<ParticleManager>();
	if(manager == nullptr) {
		LOG_ERROR("Cannot resize particles: {} is not attached to an {}", NAMEOF(ParticleManager), NAMEOF(Entity));
		return false;
	}

	// Make sure all systems are available
	if(IGlobalEventsSystem::Get() == nullptr) {
		LOG_ERROR("Cannot resize particles: {} is not registered", NAMEOF(Systems::IGlobalEventsSystem));
		return false;
	}
	if(ITimeSystem::Get() == nullptr) {
		LOG_ERROR("Cannot resize particles: {} is not registered", NAMEOF(Systems::ITimeSystem));
		return false;
	}

	// Bind to events
	IGlobalEventsSystem::Get()->bind(Global::Update, &updateListener);
	return true;
}

const std::string_view &ParticleSizeUpdater::NameClass() const {
	static constexpr std::string_view className = NAMEOF(RassEngine::Components::Particles::ParticleSizeUpdater);
	return className;
}

bool ParticleSizeUpdater::Read(Stream &stream) {
	// Make sure stream is valid
	if(!Component::Read(stream)) {
		return false;
	}

	// Read the node values
	stream.PushNode(NAMEOF(ParticleSizeUpdater));

	// Read the curve attributes
	curve.Read(stream);

	stream.PopNode();
	return true;
}

#pragma endregion Public Functions

//--------------------------------------------------------------------------
// Private Functions:
//--------------------------------------------------------------------------

#pragma region Private Functions

bool ParticleSizeUpdater::Update(const IEvent<GlobalEventArgs> *, const GlobalEventArgs &) {
	float dt = ITimeSystem::Get()->GetDeltaTimeSec();

	// Adjust the scale for each particle
	manager->ForEachActiveParticle([this, dt] (const ParticleManager::StartingStats &startingStats, Particle &particle) {
		// Compute time
		float scalar = 1.f - particle.lifetime / startingStats.lifetime;
		scalar = curve.Calculate(scalar);

		// Update the particle's scale
		particle.scale = startingStats.scale * scalar;
	});
	return true;
}

#pragma endregion Private Functions

}	// namespace
