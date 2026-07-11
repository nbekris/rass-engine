#include <Precompiled.h>
#include "SpawnParticlesAction.h"

#include <memory>
#include <Entity.h>
#include <Stream.h>
#include <Components/Transform.h>
#include <Systems/Entity/IEntityFactory.h>
#include <Systems/Scene/ISceneSystem.h>

using namespace RassEngine;
using namespace RassEngine::Systems;

namespace RassGame::Systems {
bool SpawnParticlesAction::Read(Stream &stream) {
	stream.Read("Prefab", prefabPath);
	stream.ReadVec3("Offset", offset);
	return true;
}
void SpawnParticlesAction::Execute(const FeelContext &ctx) {
	auto *factory = IEntityFactory::Get();
	auto *scene = ISceneSystem::Get();
	if(factory == nullptr || scene == nullptr || prefabPath.empty()) {
		return;
	}
	// Build an independent entity — unaffected by the (possibly destroyed) source entity.
	std::unique_ptr<Entity> entity = factory->Build(prefabPath);
	if(entity == nullptr) {
		return;
	}
	if(auto *transform = entity->GetTransform()) {
		transform->SetPosition(ctx.position + offset);   // snapshot position, not a live lookup
	}
	scene->AddEntity(std::move(entity));   // same path ShootingController uses for bullets
}
}
