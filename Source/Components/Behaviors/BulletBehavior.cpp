// File Name:    BulletBehavior.cpp
// Author(s):    main Boyuan Hu, secondary Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Behavior component controlling bullet movement and lifetime.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "BulletBehavior.h"

#include <cmath>

#include "Cloneable.h"
#include "Component.h"
#include "Components/PhysicsBody.h"
#include "Components/Transform.h"
#include "Components/Collider.h"
#include "Entity.h"
#include "Events/Global.h"
#include "Events/GlobalEventArgs.h"
#include "IEvent.h"
#include "Stream.h"

#include "Events/AudioEvents.h"
#include "Systems/GlobalEvents/IGlobalEventsSystem.h"
#include "Systems/Logging/ILoggingSystem.h"
#include "Systems/Physics/IPhysicsSystem.h"
#include "Systems/Time/ITimeSystem.h"

namespace RassEngine::Components::Behaviors {
static constexpr float MIN_MOVING_SPEED = 1.0f;
static constexpr float STOPPED_THRESHOLD = 0.5f;
// Probe distance ahead of the bullet center used to identify the hit tile.
static constexpr float TILE_PROBE_DIST = 0.2f;
BulletBehavior::BulletBehavior() : Cloneable<Component, BulletBehavior>{}
, onUpdateListener{this, &BulletBehavior::Update} {}

BulletBehavior::BulletBehavior(const BulletBehavior &other)
	: Cloneable<Component, BulletBehavior>{other}
	, onUpdateListener{this, &BulletBehavior::Update}
	, damage{other.damage}
	, destroyOnImpact{other.destroyOnImpact}
	, canCrossColliders{other.canCrossColliders}
	, isEnemyBullet{other.isEnemyBullet}
	, destroysTiles{other.destroysTiles}
	, lifetime{other.lifetime}
	, bounces{other.bounces}
	, speed{other.speed}
	, destroyableTileTypes{other.destroyableTileTypes}
	, tileDestroySoundFile{other.tileDestroySoundFile}
{}

BulletBehavior::~BulletBehavior() {
	if(Systems::IGlobalEventsSystem::Get() == nullptr) {
		return;
	}
	Systems::IGlobalEventsSystem::Get()->unbind(Events::Global::Update, &onUpdateListener);
}

bool BulletBehavior::Initialize() {
	if(Systems::IGlobalEventsSystem::Get() == nullptr) {
		LOG_ERROR("Cannot bind to {}", NAMEOF(Systems::IGlobalEventsSystem));
		return false;
	} else if(Systems::ITimeSystem::Get() == nullptr) {
		LOG_ERROR("Cannot retrieve {}", NAMEOF(Systems::ITimeSystem));
		return false;
	} else if(Parent() == nullptr) {
		LOG_ERROR("Unable to retrieve parent");
		return false;
	} else if(Parent()->GetTransform() == nullptr) {
		LOG_ERROR("Unable to retrieve transform");
		return false;
	}

	// Cache probe distances once; avoids dynamic_cast on every Update tick.
	// Falls back to TILE_PROBE_DIST if no collider is present.
	if(const auto *col = Parent()->Get<Components::Collider>()) {
		cachedProbeX = col->GetSize().x * 0.5f + TILE_PROBE_DIST;
		cachedProbeY = col->GetSize().y * 0.5f + TILE_PROBE_DIST;
	}

	if(!tileDestroySoundFile.empty()) {
		Events::AudioEvents::LoadSound(tileDestroySoundFile);
	}

	Systems::IGlobalEventsSystem::Get()->bind(Events::Global::Update, &onUpdateListener);
	return true;
}

const std::string_view &BulletBehavior::NameClass() const {
	static constexpr std::string_view className = NAMEOF(Components::Behaviors::BulletBehavior);
	return className;
}

bool BulletBehavior::Read(Stream &stream) {
	if(!Component::Read(stream)) {
		return false;
	}
	stream.Read("Damage", damage);
	stream.Read("DestroyOnImpact", destroyOnImpact);
	stream.Read("CanCrossColliders", canCrossColliders);
	stream.Read("IsEnemyBullet", isEnemyBullet);
	stream.Read("DestroyTiles", destroysTiles);
	stream.Read("TileDestroySoundFile", tileDestroySoundFile);
	stream.Read("Lifetime", lifetime);
	stream.Read("Bounces", bounces);
	stream.Read("Speed", speed);
	stream.ReadArray("DestroyableTileTypes", [&] () {
		unsigned tileIndex = 0;
		if(stream.Read("Index", tileIndex)) {
			destroyableTileTypes.insert(tileIndex);
		}
		});
	return true;
}

bool BulletBehavior::Update(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &) {
	float dt = Systems::ITimeSystem::Get()->GetDeltaTimeSec();
	timer += dt;
	if(timer > lifetime) {
		Parent()->Destroy();
		return true;
	}

	auto *body = Parent()->Get<Components::PhysicsBody>();
	auto *trans = Parent()->GetTransform();

	if(body && trans) {
		const glm::vec3 vel3d = body->GetVelocity();
		const glm::vec2 vel2d(vel3d.x, vel3d.y);

		// Detect which velocity components were cancelled by a tile this frame.
		// PhysicsSystem (FixedUpdate) zeroes the component on tile contact.
		// prevVelocity captures the last-Update value, which predates the zeroing.
		// This is robust against SEPARATION_EPSILON race conditions:
		// velocity stays at 0 regardless of how many FixedUpdates run before this Update.
		const bool hitVertical = (std::abs(prevVelocity.x) > MIN_MOVING_SPEED)
			&& (std::abs(vel2d.x) < STOPPED_THRESHOLD);
		const bool hitHorizontal = (std::abs(prevVelocity.y) > MIN_MOVING_SPEED)
			&& (std::abs(vel2d.y) < STOPPED_THRESHOLD);
		const bool hitAny = hitVertical || hitHorizontal;

		if(hitAny) {
			const glm::vec3 pos = trans->GetPosition();
			auto *col = Parent()->Get<Components::Collider>();

			if(destroysTiles) {
				auto *physics = Systems::IPhysicsSystem::Get();
				bool destroyed = false;

				if(hitHorizontal) {
					const float signY = (prevVelocity.y < 0.0f) ? -1.0f : 1.0f;
					destroyed = physics->DestroyTileAtWorldPos(pos.x, pos.y + signY * cachedProbeY,
						destroyableTileTypes);
				}
				if(!destroyed && hitVertical) {
					const float signX = (prevVelocity.x > 0.0f) ? 1.0f : -1.0f;
					destroyed = physics->DestroyTileAtWorldPos(pos.x + signX * cachedProbeX, pos.y,
						destroyableTileTypes);
				}

				if(destroyed && !tileDestroySoundFile.empty()) {
					Events::AudioEvents::PlaySFX(tileDestroySoundFile);
				}
				if(destroyed && destroyOnImpact) {
					Parent()->Destroy();
					return true;
				}
			}

			// tile bouncing — unchanged
			if(bounces > 0) {
				glm::vec2 newVel = vel2d;
				if(hitVertical)   newVel.x = -prevVelocity.x;
				if(hitHorizontal) newVel.y = -prevVelocity.y;
				body->SetVelocity(glm::vec3(newVel.x, newVel.y, vel3d.z));
				--bounces;
				if(bounces <= 0) {
					Parent()->Destroy();
					return true;
				}
			}
			else if(destroyOnImpact) {
				Parent()->Destroy();
				return true;
			}
		}

		// Always update prevVelocity at the END of the tick.
		// After a hit, both prevVelocity components are near 0 → no re-trigger next frame.
		prevVelocity = vel2d;
	}

	return true;
}

}
