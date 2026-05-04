// File Name:    ParticleMover.h
// Author(s):    main Taro Omiya, secondary Steven Yacoub, Niko Bekris, Eric Fleegal
// Course:       GAM541
// Project:      RASS
// Purpose:      File stream utilities for reading and writing data.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include "Component.h"
#include "Cloneable.h"
#include "Events/GlobalEventArgs.h"
#include "Events/GlobalEventListener.h"
#include "IEvent.h"

// Forward Declarations:
namespace RassEngine {
class Stream;
}

namespace RassEngine::Components::Particles {
// Forward Declarations:
class ParticleManager;

// Class Definition:
class ParticleMover : public Cloneable<Component, ParticleMover> {
public:
	ParticleMover(void);

	// @brief This copy-constructor should perform a shallow copy of the data.
	ParticleMover(const ParticleMover &other);
	virtual ~ParticleMover(void) override;

	// @brief Initialize the component.
	// @return bool = true if initialization successful, otherwise false.
	bool Initialize() override;

	// Inherited via Cloneable
	const std::string_view &NameClass() const override;

	// @brief Read the properties of a ParticleMover component from a stream.
	// @param stream = The data stream used for reading.
	bool Read(Stream &stream) override;

	// Private Functions:
private:
	bool Update(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &);

	Events::GlobalEventListener<ParticleMover> updateListener;
	ParticleManager *manager = nullptr;
};

}	// namespace
