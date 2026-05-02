// File Name:    ParticleManager.h
// Author(s):    main Taro Omiya, secondary Steven Yacoub, Niko Bekris, Eric Fleegal
// Course:       GAM541
// Project:      RASS
// Purpose:      File stream utilities for reading and writing data.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <functional>
#include <vector>
#include <tuple>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "Component.h"
#include "Cloneable.h"
#include "Events/GlobalEventArgs.h"
#include "Events/GlobalEventListener.h"
#include "IEvent.h"
#include "Particle.h"

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
class ParticleManager : public Cloneable<Component, ParticleManager> {
public:
	// Starting stats
	struct StartingStats {
		// The duration this particle is intended to remain alive
		float lifetime{0};

		// The "physics" value(s).
		glm::vec3 velocity{0.f, 0.f, 0.f};
		float angularVelocityRad{0.f};

		// The other starting stats
		glm::vec4 color{1.f, 1.f, 1.f, 1.f};
		glm::vec3 scale{1.f, 1.f, 1.f};
	};

	using particleUpdater = std::function<void(const StartingStats &startingStats, Particle &particle)>;

	ParticleManager(void);

	// @brief This copy-constructor should perform a shallow copy of the data.
	ParticleManager(const ParticleManager &other);
	virtual ~ParticleManager(void) override;

	// @brief Initialize the component.
	// @return bool = true if initialization successful, otherwise false.
	bool Initialize() override;

	// Inherited via Cloneable
	const std::string_view &NameClass() const override;

	// @brief Read the properties of a ParticleManager component from a stream.
	// @param stream = The data stream used for reading.
	bool Read(Stream& stream) override;

	// Return true if particleActive == 0.
	inline bool IsEmpty() const {
		return particleActive == 0;
	}

	// Return true if particleFree == 0.
	inline bool IsFull() const {
		return particleFree == 0;
	}

	// Allocate an unused particle, if possible.
	std::tuple<StartingStats *, Particle *> AllocateParticle();

	// TODO: replace this with an iterator
	void ForEachActiveParticle(particleUpdater lambda);

	struct ParticleData {
		StartingStats starting{};
		Particle current{};
	};

	// Private Functions:
private:

	bool Update(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &);
	bool Render(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &);

	// Kill an active particle.
	// [NOTE: When areRecyclable is true, particles become "Free", instead of "Dead".]
	void KillParticle(unsigned particleIndex);

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

	Graphics::Mesh *mesh{nullptr};
	Graphics::Texture *texture{nullptr};
	Events::GlobalEventListener<ParticleManager> updateListener;
	Events::GlobalEventListener<ParticleManager> renderListener;
};

}	// namespace
