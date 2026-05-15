#pragma once

//------------------------------------------------------------------------------
// Includes:
//------------------------------------------------------------------------------

#include "IParticleSystem.h"

#include <string_view>

namespace RassEngine::Systems {

class ParticleSystem : public IParticleSystem {
	// Constructors/Destructors:
public:
	inline ParticleSystem() = default;
	inline virtual ~ParticleSystem(void) override = default;

	// Inherited via ITimeSystem
	bool Initialize() override;
	const std::string_view &NameClass() const override;
	void Shutdown() override;
};

}	// namespace
