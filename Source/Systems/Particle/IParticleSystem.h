#pragma once

#include "Systems/GlobalEvents/IGlobalSystem.h"

namespace RassEngine::Systems {

class IParticleSystem : public IGlobalSystem<IParticleSystem> {
public:
	virtual ~IParticleSystem() = 0;
};

}
