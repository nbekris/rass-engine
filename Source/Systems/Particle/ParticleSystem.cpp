#include "Precompiled.h"

#include <string_view>

#include "Component.h"
#include "Components/Particles/EmitterBox.h"
#include "Components/Particles/EmitterCone.h"
#include "Components/Particles/ParticleAlphaUpdater.h"
#include "Components/Particles/ParticleEmitter.h"	
#include "Components/Particles/ParticleManager.h"
#include "Components/Particles/ParticleMover.h"
#include "Components/Particles/ParticleSizeUpdater.h"
#include "ParticleSystem.h"
#include "Systems/Component/IComponentFactory.h"
#include "Utils.h"

//------------------------------------------------------------------------------
// External Declarations:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Namespace Declarations:
//------------------------------------------------------------------------------

using namespace RassEngine::Components::Particles;

namespace RassEngine::Systems {

bool ParticleSystem::Initialize() {
	// Add system-specific initialization code here.

	// Register the "ParticleEmitter" and "BehaviorCursor" components.
	IComponentFactory::Get()->Register(RemoveNamespace(NAMEOF(ParticleEmitter)), [] () -> Component * {
		return new ParticleEmitter();
	});
	IComponentFactory::Get()->Register(RemoveNamespace(NAMEOF(ParticleMover)), [] () -> Component * {
		return new ParticleMover();
	});
	IComponentFactory::Get()->Register(RemoveNamespace(NAMEOF(ParticleSizeUpdater)), [] () -> Component * {
		return new ParticleSizeUpdater();
	});
	IComponentFactory::Get()->Register(RemoveNamespace(NAMEOF(ParticleAlphaUpdater)), [] () -> Component * {
		return new ParticleAlphaUpdater();
	});
	IComponentFactory::Get()->Register(RemoveNamespace(NAMEOF(ParticleManager)), [] () -> Component * {
		return new ParticleManager();
	});
	IComponentFactory::Get()->Register(RemoveNamespace(NAMEOF(EmitterBox)), [] () -> Component * {
		return new EmitterBox();
	});
	IComponentFactory::Get()->Register(RemoveNamespace(NAMEOF(EmitterCone)), [] () -> Component * {
		return new EmitterCone();
	});

	// Return true if the initialization completed successfully.
	return true;
}

const std::string_view &ParticleSystem::NameClass() const {
	static constexpr std::string_view className = NAMEOF(RassEngine::Systems::ParticleSystem);
	return className;
}

void ParticleSystem::Shutdown() {
	// Do nothing
}

}	// namespace
