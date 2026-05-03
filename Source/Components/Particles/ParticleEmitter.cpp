// File Name:    ParticleEmitter.cpp
// Author(s):    main Taro Omiya, secondary Steven Yacoub, Niko Bekris, Eric Fleegal
// Course:       GAM541
// Project:      RASS
// Purpose:      File stream utilities for reading and writing data.
//
// Copyright © 2026 DigiPen (USA) Corporation.---------------------------------------------------------------------------

#include "Precompiled.h"
#include "ParticleEmitter.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <memory>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

#include "Cloneable.h"
#include "Component.h"
#include "Components/Transform.h"
#include "EmitterShape.h"
#include "Entity.h"
#include "Events/Global.h"
#include "Events/GlobalEventArgs.h"
#include "IEvent.h"
#include "Particle.h"
#include "ParticleManager.h"
#include "Random.h"
#include "Stream.h"
#include "Systems/Component/IComponentFactory.h"
#include "Systems/GlobalEvents/IGlobalEventsSystem.h"
#include "Systems/Logging/ILoggingSystem.h"
#include "Systems/Time/ITimeSystem.h"
#include "TweenCurve.h"
#include "Utils.h"

using namespace RassEngine::Systems;
using namespace RassEngine::Events;
using namespace RassEngine::Graphics;

namespace RassEngine::Components::Particles {

static const char *PARTICLE_EMITTER = "ParticleEmitter";
static const char *LOG_TRANSFORM = "transform";
static const char *LOG_MANAGER = "ParticleManager";

#pragma region Constructors

ParticleEmitter::ParticleEmitter(void)
	: Cloneable<Component, ParticleEmitter>()
	, updateListener{this, &ParticleEmitter::Update}
{}

ParticleEmitter::ParticleEmitter(const ParticleEmitter &other)
	: Cloneable<Component, ParticleEmitter>{other}
	, emitRate(other.emitRate)
	, lifetimeMin(other.lifetimeMin)
	, lifetimeMax(other.lifetimeMax)
	, scaleMin(other.scaleMin)
	, scaleMax(other.scaleMax)
	, tintColor(other.tintColor)
	, isEmitting(other.isEmitting)
	, updateListener{this, &ParticleEmitter::Update}
{}

ParticleEmitter::~ParticleEmitter(void) {
	if(IGlobalEventsSystem::Get() == nullptr) {
		return;
	}

	// Unbind from the event
	IGlobalEventsSystem::Get()->unbind(Global::Update, &updateListener);
}

#pragma endregion Constructors

#pragma region Public Functions

bool ParticleEmitter::Initialize() {
	// Retrieve the particle manager
	if(Parent() == nullptr) {
		LOG_ERROR("Cannot emit particles: {} is not attached to an {}", NameClass(), NAMEOF(Entity));
		return false;
	}

	manager = Parent()->Get<ParticleManager>();
	if(manager == nullptr) {
		LOG_ERROR("Cannot emit particles: {} is not attached to an {}", NAMEOF(ParticleManager), NAMEOF(Entity));
		return false;
	}

	shape = Parent()->Get<EmitterShape>();
	if(shape == nullptr) {
		LOG_ERROR("Cannot emit particles: {} is not attached to an {}", NAMEOF(EmitterShape), NAMEOF(Entity));
		return false;
	}

	// Make sure all systems are available
	if(IGlobalEventsSystem::Get() == nullptr) {
		LOG_ERROR("Cannot emit particles: {} is not registered", NAMEOF(Systems::IGlobalEventsSystem));
		return false;
	}
	if(ITimeSystem::Get() == nullptr) {
		LOG_ERROR("Cannot emit particles: {} is not registered", NAMEOF(Systems::ITimeSystem));
		return false;
	}

	// Bind to events
	IGlobalEventsSystem::Get()->bind(Global::Update, &updateListener);
	return true;
}

const std::string_view &ParticleEmitter::NameClass() const {
	static constexpr std::string_view className = NAMEOF(RassEngine::Components::Particles::ParticleEmitter);
	return className;
}

bool ParticleEmitter::Read(Stream &stream) {
	// Make sure stream is valid
	if(!Component::Read(stream)) {
		return false;
	}

	// Read the node values
	stream.PushNode(NAMEOF(ParticleEmitter));

	// Read the emitter properties, from "emitRate" to "scale".
	stream.Read("EmitOnStart", isEmitting);
	stream.Read("EmitRate", emitRate);
	stream.Read("LifeTimeMin", lifetimeMin);
	stream.Read("LifeTimeMax", lifetimeMax);
	stream.Read("ScaleMin", scaleMin);
	stream.Read("ScaleMax", scaleMax);

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
		Component *toConvert = IComponentFactory::Get()->Create(key);
		if(toConvert == nullptr) {
			LOG_ERROR("{}: unable to create object, {}", NAMEOF(ParticleEmitter), key);
			return;
		}

		// See if this can be converted into a component
		shape = dynamic_cast<Particles::EmitterShape *>(toConvert);
		if(shape == nullptr) {
			// Clean up the object
			delete toConvert;

			// Log that object wasn't a component
			LOG_ERROR("{}: unable to create object, {}", NAMEOF(ParticleEmitter), key);
			return;
		}

		// If so, read from the stream
		shape->Read(stream);

		// Add the shape as a component to the entity
		Parent()->AddComponent(std::unique_ptr<Particles::EmitterShape>(shape));
	});

	// Return to the original location in the tree.
	stream.PopNode();
	return true;
}

#pragma endregion Public Functions

#pragma region Private Functions

bool ParticleEmitter::Update(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &) {
	float dt = ITimeSystem::Get()->GetDeltaTimeSec();

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
	return true;
}

void ParticleEmitter::SetupParticle(ParticleManager::StartingStats *startingStats, Particle *particle) {
	// If a particle was successfully allocated,
	if((startingStats == nullptr) || (particle == nullptr)) {
		return;
	}

	// Initialize the lifetime to a random value between lifetimeMin and lifetimeMax.
	startingStats->lifetime = Random::range(lifetimeMin, lifetimeMax);
	particle->lifetime = startingStats->lifetime;

	// Setup random color
	startingStats->color = glm::vec4(Random::range(0.f, tintColor.r), Random::range(0.f, tintColor.g), Random::range(0.f, tintColor.b), tintColor.a);
	particle->color = startingStats->color;

	// Set the particle's scale
	startingStats->scale = TweenCurve::Calculate(TweenCurve::Type::Linear, scaleMin, scaleMax, Random::range(0.f, 1.f));
	particle->scale = startingStats->scale;

	// Confirm we have a shape
	if(shape == nullptr) {
		return;
	}

	// Get the parent entity's Transform.
	const Transform *transform = Parent()->GetTransform();
	if(transform == nullptr) {
		return;
	}

	// Initialize the particle's position and rotationRad
	std::tie(particle->position, particle->rotationRad) = shape->GetEmitTransform(*transform);

	// Compute the particle's velocities
	std::tie(startingStats->velocity, startingStats->angularVelocityRad) = shape->GetInitVelocities(*particle);
}

#pragma endregion Private Functions

}	// namespace
