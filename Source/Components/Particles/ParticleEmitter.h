// File Name:    ParticleEmitter.h
// Author(s):    main Taro Omiya, secondary Steven Yacoub, Niko Bekris, Eric Fleegal
// Course:       GAM541
// Project:      RASS
// Purpose:      File stream utilities for reading and writing data.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <string_view>

#include "Cloneable.h"
#include "Component.h"
#include "EmitterShape.h"
#include "Events/GlobalEventArgs.h"
#include "Events/GlobalEventListener.h"
#include "IEvent.h"
#include "Particle.h"
#include "ParticleManager.h"

// Forward Declarations:
namespace RassEngine {
class Stream;
}

namespace RassEngine::Graphics {
class Mesh;
class Texture;
}

namespace RassEngine::Components::Particles {

// Class Definition:
class ParticleEmitter : public Cloneable<Component, ParticleEmitter> {
public:
	ParticleEmitter(void);

	// @brief This copy-constructor should perform a shallow copy of the data.
	ParticleEmitter(const ParticleEmitter &other);
	virtual ~ParticleEmitter(void) override;

	// Public Functions:
public:
	// @brief Initialize the component.
	// @return bool = true if initialization successful, otherwise false.
	bool Initialize() override;

	// Inherited via Cloneable
	const std::string_view &NameClass() const override;

	// @brief Read the properties of a ParticleEmitter component from a stream.
	// @param stream = The data stream used for reading.
	bool Read(Stream &stream) override;

	inline bool IsEmitting() const {
		return isEmitting;
	}

	inline void IsEmitting(bool setToEmit) {
		isEmitting = setToEmit;
	}

	// Private Functions:
private:
	// @brief Update the component each frame.
	bool Update(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &);

	void SetupParticle(ParticleManager::StartingStats *startingStats, Particle *particle);

	// Private Variables:
private:
	// Tracks the number of particles to emit, including "partial" particles.
	// This approach works with both large and fractional numbers.
	float emitAccumulator{0.0f};

	bool isEmitting{true};
	float emitRate{1.0f};
	float lifetimeMin{1.0f};
	float lifetimeMax{1.0f};
	glm::vec3 scaleMin{1.0f, 1.0f, 1.0f};
	glm::vec3 scaleMax{1.0f, 1.0f, 1.0f};
	glm::vec4 tintColor{1.0f, 1.0f, 1.0f, 1.0f};
	EmitterShape *shape{nullptr};
	ParticleManager *manager{nullptr};
	Events::GlobalEventListener<ParticleEmitter> updateListener;
};

}	// namespace
