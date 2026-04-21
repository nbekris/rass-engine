// File Name:    PhysicsSystem.cpp
// Author(s):    main Boyuan Hu, secondary Niko Bekris, Taro Omiya, Eric Fleegal, Steven Yacoub
// Course:       GAM541
// Project:      RASS
// Purpose:      System simulating physics interactions between entities.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "PhysicsSystem.h"

#include <algorithm>
#include <cstdlib>
#include <glm/vec3.hpp>

#include "Components/Collider.h"
#include "Components/PhysicsBody.h"
#include "Components/TileMap.h"
#include "Components/Transform.h"
#include "Components/Trigger.h"

#include "Entity.h"
#include "Events/Global.h"
#include "Events/GlobalEventArgs.h"
#include "Events/SceneChange.h"
#include "Events/TriggerEventIDs.h"
#include "Events/TriggerEventArgs.h"
#include "IEvent.h"
#include "TileMapData.h"
#include "Systems/GlobalEvents/IGlobalEventsSystem.h"
#include "Systems/Logging/ILoggingSystem.h"
#include "Systems/Time/ITimeSystem.h"

namespace RassEngine::Systems {
//utility function to check if a push in a certain direction would push the body into a tile, based on its current binarycollision states
static bool WouldPushIntoTile(const Components::PhysicsBody *body, const glm::vec3 &pushDir) {
	if(body->isGrounded && pushDir.y < 0.0f) return true;
	if(body->isTouchingCeiling && pushDir.y > 0.0f) return true;
	if(body->isTouchingLeftWall && pushDir.x < 0.0f)return true;
	if(body->isTouchingRightWall && pushDir.x > 0.0f)return true;
	return false;
}
PhysicsSystem::PhysicsSystem()
	: updateListener{this, &PhysicsSystem::Update}
	, gravity(0.0f, -9.8f, 0.0f)
	, onSceneShutdown{[this](const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &) {
		CleanUp();
		return true;
	}} {}

PhysicsSystem::~PhysicsSystem() {
	Shutdown();
	CleanUp();
}

bool PhysicsSystem::Initialize() {
	auto *events = IGlobalEventsSystem::Get();
	if(events == nullptr) {
		LOG_WARNING("{}: Could not initialize IGlobal Events System", NameClass());
		return false;
	}

	// bind to the update event
	events->bind(Events::Global::FixedUpdate, &updateListener);
	events->bind(Events::SceneChange::BeforeShutdown, &onSceneShutdown);
	return true;
}

void PhysicsSystem::Shutdown() {
	auto *events = IGlobalEventsSystem::Get();
	if(events != nullptr) {
		events->unbind(Events::Global::FixedUpdate, &updateListener);
		events->unbind(Events::SceneChange::BeforeShutdown, &onSceneShutdown);
	}
}

void PhysicsSystem::CleanUp() {
	physicsProxies.clear();
	triggerStates.clear();
}

bool PhysicsSystem::Update(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &) {
	float dt = Systems::ITimeSystem::Get()->GetFixedDeltaTimeSec();

	std::vector<Entity *> entities; // Placeholder for actual entity retrieval
	for(auto &proxy : physicsProxies) {
		if(proxy.physicsBody) {
			proxy.physicsBody->UpdatePreviousPosition(proxy.transform->GetPosition());
		}
	}
	IntegrateBodies(dt, entities);
	CheckTileCollisions(dt);
	DetectCollisions(entities);
	DetectTriggers(entities);
	return true;
}

void PhysicsSystem::IntegrateBodies(float dt, std::vector<Entity *> &) {
	// Loop through cache of physics proxies and only move those with physics bodies
	for(auto &proxy : physicsProxies) {
		auto body = proxy.physicsBody;
		if(body) {
			glm::vec3 velocity = body->GetVelocity();

			if(!body->isKinematic) {
				velocity += gravity * dt;

				velocity *= (1.0f - body->GetDrag() * dt);
			}

			glm::vec3 currentPos = proxy.transform->GetPosition();
			body->SetVelocity(velocity);
			proxy.transform->SetPosition(currentPos + (velocity * dt));
		}
	}
}
void PhysicsSystem::CheckTileCollisions(float dt) {
	if(tileMaps.empty()) return;

	for(auto &proxy : physicsProxies) {
		if(!proxy.physicsBody) continue;
		if(!proxy.collider) continue;

		const glm::vec3 &colliderSize = proxy.collider->GetSize();
		const glm::vec3 &colliderOffset = proxy.collider->GetOffset();
		glm::vec3 worldPos = proxy.transform->GetPosition() + colliderOffset;
		
		// Reset collision states
		proxy.physicsBody->isGrounded = false;
		proxy.physicsBody->isTouchingCeiling = false;
		proxy.physicsBody->isTouchingLeftWall = false;
		proxy.physicsBody->isTouchingRightWall = false;

		for(auto *tileMap : tileMaps) {
			const TileMapData *tileMapData = tileMap->GetTileMapData();
			if(!tileMapData) continue;

			glm::vec3 tileMapWorldPos(0.0f);
			glm::vec3 tileMapScale(1.0f);
			if(auto *parent = tileMap->Parent()) {
				if(auto *tileMapTransform = parent->Get<Components::Transform>()) {
					tileMapWorldPos = tileMapTransform->GetPosition();
					tileMapScale = tileMapTransform->GetLocalScale();
				}
			}
			// First do a quick check with combined flags, then do axis-by-axis resolution if there's a collision
			glm::vec3 localPos = (worldPos - tileMapWorldPos) / tileMapScale;
			float     localW = colliderSize.x / tileMapScale.x;
			float     localH = colliderSize.y / tileMapScale.y;
			int combinedFlags = tileMapData->CheckAreaCollision(
				localPos.x, localPos.y, localW, localH
			);

			if(TileMapData::HasCollision(combinedFlags)) {
				ResolveTileCollision(proxy, tileMapData, tileMapWorldPos, tileMapScale, dt);
			}
		}
	}
}

void PhysicsSystem::ResolveTileCollision(PhysicsProxy &proxy, const TileMapData *tileMapData, const glm::vec3 &tileMapWorldPos, const glm::vec3 &tileMapScale, float dt) {
	static constexpr float SEPARATION_EPSILON = 0.001f;

	glm::vec3 velocity = proxy.physicsBody->GetVelocity();
	glm::vec3 currentPos = proxy.transform->GetPosition();
	glm::vec3 oldPos = proxy.physicsBody->previousPosition;

	const glm::vec3 &colliderSize = proxy.collider->GetSize();
	const glm::vec3 &colliderOffset = proxy.collider->GetOffset();

	float localW = colliderSize.x / tileMapScale.x;
	float localH = colliderSize.y / tileMapScale.y;

	//x axis check: use new X + old Y to avoid ground/ceiling interference
	glm::vec3 xWorldCenter = glm::vec3(currentPos.x, oldPos.y, currentPos.z) + colliderOffset;
	glm::vec3 xLocalCenter = (xWorldCenter - tileMapWorldPos) / tileMapScale;
	int xFlags = tileMapData->CheckAreaCollision(xLocalCenter.x, xLocalCenter.y, localW, localH);

	// Track whether X was corrected; used to pick a safe X for the Y-axis check below
	bool xCorrected = false;

	// State is set based on geometry; position/velocity correction only when moving into the surface
	if(TileMapData::IsTouchingLeftWall(xFlags)) {
		proxy.physicsBody->isTouchingLeftWall = true;
		if(velocity.x <= 0.0f) {
			currentPos.x = oldPos.x + SEPARATION_EPSILON; // Push slightly away from left wall
			velocity.x = 0.0f;
			xCorrected = true;
		}
	}
	if(TileMapData::IsTouchingRightWall(xFlags)) {
		proxy.physicsBody->isTouchingRightWall = true;
		if(velocity.x >= 0.0f) {
			currentPos.x = oldPos.x - SEPARATION_EPSILON; // Push slightly away from right wall
			velocity.x = 0.0f;
			xCorrected = true;
		}
	}

	// Y axis check: when X was corrected, use the corrected currentPos.x to prevent wall tile
	// corners from being falsely detected as floor/ceiling; otherwise use oldPos.x for axis separation
	float yCheckX = xCorrected ? currentPos.x : oldPos.x;
	glm::vec3 yWorldCenter = glm::vec3(yCheckX, currentPos.y, currentPos.z) + colliderOffset;
	glm::vec3 yLocalCenter = (yWorldCenter - tileMapWorldPos) / tileMapScale;
	int yFlags = tileMapData->CheckAreaCollision(yLocalCenter.x, yLocalCenter.y, localW, localH);

	if(TileMapData::IsStanding(yFlags)) {
		proxy.physicsBody->isGrounded = true;
		if(velocity.y <= 0.0f) {
			currentPos.y = oldPos.y; // Snap to last safe Y (no epsilon to avoid upward drift)
			velocity.y = 0.0f;
		}
	}
	if(TileMapData::IsTouchingCeiling(yFlags)) {
		proxy.physicsBody->isTouchingCeiling = true;
		if(velocity.y > 0.0f) {
			currentPos.y = oldPos.y; // Snap to last safe Y (no epsilon to avoid downward drift)
			velocity.y = 0.0f;
		}
	}

	// Ground friction: apply Coulomb friction
	if(proxy.physicsBody->isGrounded && std::abs(velocity.x) > 0.0f) {
		float frictionCoeff = proxy.physicsBody->friction;
		float deceleration = frictionCoeff * std::abs(gravity.y) * dt;
		if(std::abs(velocity.x) <= deceleration) {
			velocity.x = 0.0f;
		} else {
			velocity.x -= std::copysign(deceleration, velocity.x);
		}
	}

	proxy.transform->SetPosition(currentPos);
	proxy.physicsBody->SetVelocity(velocity);
}


void PhysicsSystem::DetectCollisions(std::vector<Entity *> &) {
	for(size_t i = 0; i < physicsProxies.size(); ++i) {
		PhysicsProxy &proxyA = physicsProxies[i];

		// Skip trigger-only colliders — they don't participate in physics resolution
		if(proxyA.collider && proxyA.collider->IsTrigger()) {
			continue;
		}

		for(size_t j = i + 1; j < physicsProxies.size(); ++j) {
			PhysicsProxy &proxyB = physicsProxies[j];

			// Skip trigger-only colliders
			if(proxyB.collider && proxyB.collider->IsTrigger()) {
				continue;
			}

			// Optimization: If both are static (no physics body), skip.
			if(proxyA.physicsBody == nullptr && proxyB.physicsBody == nullptr) {
				continue;
			}

			if(proxyA.collider->Intersects(
				*proxyB.collider,
				proxyA.transform->GetPosition(),
				proxyB.transform->GetPosition())
				) {
				ResolveCollisions(proxyA, proxyB);
			}
		}
	}
}

void PhysicsSystem::ResolveCollisions(PhysicsProxy &a, PhysicsProxy &b) {
	if(!a.collider || !b.collider) return;

	// Manifold Values
	glm::vec3 normal(0.0f);
	float penetration = 0.0f;

	bool isColliding = false;

	if(a.collider->type == Components::Collider::ColliderType::Box
		&& b.collider->type == Components::Collider::ColliderType::Box) {
		auto boundsA = a.collider->GetBoxBounds(a.transform->GetPosition());
		auto boundsB = b.collider->GetBoxBounds(b.transform->GetPosition());

		float xOverlap = GetOverlap(boundsA.min.x, boundsA.max.x, boundsB.min.x, boundsB.max.x);
		float yOverlap = GetOverlap(boundsA.min.y, boundsA.max.y, boundsB.min.y, boundsB.max.y);

		if(xOverlap > 0.0f && yOverlap > 0.0f) {
			isColliding = true;
			bool separateX = std::abs(xOverlap) < std::abs(yOverlap);

			if(separateX) {
				penetration = std::abs(xOverlap);
				normal.x = (a.transform->GetPosition().x < b.transform->GetPosition().x) ? 1.0f : -1.0f;
			} else {
				penetration = std::abs(yOverlap);
				normal.y = (a.transform->GetPosition().y < b.transform->GetPosition().y) ? 1.0f : -1.0f;
			}
		}
	}

	else if(a.collider->type == Components::Collider::ColliderType::Circle
		&& b.collider->type == Components::Collider::ColliderType::Circle) {
		auto circleA = a.collider->GetCircleBounds(a.transform->GetPosition());
		auto circleB = b.collider->GetCircleBounds(b.transform->GetPosition());

		glm::vec3 distanceVec = circleB.center - circleA.center;
		float distance = glm::length(distanceVec);
		float radiusSum = circleA.circleRadius + circleB.circleRadius;

		if(distance < radiusSum) {
			isColliding = true;
			if(distance == 0.0f) {
				penetration = circleA.circleRadius;
				normal = glm::vec3(1.0f, 0.0f, 0.0f);
			} else {
				penetration = radiusSum - distance;
				normal = distanceVec / distance;
			}
		}
	}

	if(!isColliding) return;

	// Prevent Division by Zero for Static vs Static
	bool aIsDynamic = (a.physicsBody != nullptr && a.physicsBody->GetInverseMass() > 0.0f);
	bool bIsDynamic = (b.physicsBody != nullptr && b.physicsBody->GetInverseMass() > 0.0f);

	if(!aIsDynamic && !bIsDynamic) {
		return;
	}

	// Rely entirely on the isolated functions (No manual SetPosition!)
	ApplyImpulse(a, b, normal);
	ApplyFrictionImpulse(a, b, normal);
	PositionalCorrection(a, b, penetration, normal);
}

float PhysicsSystem::GetOverlap(float minA, float maxA, float minB, float maxB) {
	if(maxA < minB || minA > maxB) return 0.0f;
	float left = maxA - minB;
	float right = maxB - minA;
	return (left < right) ? left : right;
}

void PhysicsSystem::ApplyImpulse(PhysicsProxy &a, PhysicsProxy &b, const glm::vec3 &normal) {
	// Ensure both have physical properties
	if(!IsSimulatable(a, b)) return;

	// Calculate relative velocity
	glm::vec3 rv = b.physicsBody->GetVelocity() - a.physicsBody->GetVelocity();

	// Calculate relative velocity in terms of the normal direction
	float velAlongNormal = glm::dot(rv, normal);

	// Do not resolve if velocities are already separating
	if(velAlongNormal > 0.0f) {
		return;
	}

	// Calculate restitution
	float e = std::min(a.physicsBody->GetRestitution(), b.physicsBody->GetRestitution());

	// Fetch inverse mass to avoid division by zero
	float invMassA = a.physicsBody->GetInverseMass();
	float invMassB = b.physicsBody->GetInverseMass();

	float effectiveInvMassA = WouldPushIntoTile(a.physicsBody, -normal) ? 0.0f : invMassA;
	float effectiveInvMassB = WouldPushIntoTile(b.physicsBody, normal) ? 0.0f : invMassB;

	float totalInvMass = effectiveInvMassA + effectiveInvMassB;
	if(totalInvMass <= 0.0f)
		return;

	// impulse scalar calculation
	float j = -(1.0f + e) * velAlongNormal / totalInvMass;
	//j /= (invMassA + invMassB);

	// Apply impulse vector
	glm::vec3 impulse = normal * j;

	bool aIsKinematic = a.physicsBody->isKinematic;
	bool bIsKinematic = b.physicsBody->isKinematic;

	// Only apply velocity changes to dynamic objects (inverseMass > 0)
	// Only apply velocity changes to objects that aren't kinematic
	if(invMassA > 0.0f && effectiveInvMassA > 0.0f && !aIsKinematic) {
		a.physicsBody->GetVelocity() -= impulse * invMassA;
	}
	if(invMassB > 0.0f && effectiveInvMassB > 0.0f && !bIsKinematic) {
		b.physicsBody->GetVelocity() += impulse * invMassB;
	}
}

void PhysicsSystem::ApplyFrictionImpulse(PhysicsProxy &a, PhysicsProxy &b, const glm::vec3 &normal) {
	if(!IsSimulatable(a, b)) return;

	glm::vec3 rv = b.physicsBody->velocity - a.physicsBody->velocity;

	// Find the Tangent Vector (The direction of sliding)
	glm::vec3 tangent = rv - glm::dot(rv, normal) * normal;

	// If the tangent length is near zero, there is no sliding, so no friction
	float tangentLen = glm::length(tangent);
	if(tangentLen < 0.0001f) {
		return;
	}
	tangent /= tangentLen; // Normalize the tangent

	float invMassA = a.physicsBody->inverseMass;
	float invMassB = b.physicsBody->inverseMass;


	float effectiveInvMassA = WouldPushIntoTile(a.physicsBody, -tangent) ? 0.0f : invMassA;
	float effectiveInvMassB = WouldPushIntoTile(b.physicsBody, tangent) ? 0.0f : invMassB;

	bool aIsKinematic = a.physicsBody->isKinematic;
	bool bIsKinematic = b.physicsBody->isKinematic;

	float totalEffectiveInvMass = effectiveInvMassA + effectiveInvMassB;
	if(totalEffectiveInvMass <= 0.0f)
		return;

	// Calculate the base friction impulse (jt)
	float jt = -glm::dot(rv, tangent) / totalEffectiveInvMass;

	// Calculate the normal impulse jn
	float velAlongNormal = glm::dot(rv, normal);
	float restitution = std::min(a.physicsBody->restitution, b.physicsBody->restitution);
	float jn = -(1.0f + restitution) * velAlongNormal;
	jn /= (invMassA + invMassB);

	// Calculate the combined friction coefficient
	float mu = std::sqrt(std::pow(a.physicsBody->friction, 2) + std::pow(b.physicsBody->friction, 2));

	// Friction force cannot exceed the normal force * friction coefficient
	float maxFriction = std::abs(jn) * mu;
	jt = glm::clamp(jt, -maxFriction, maxFriction);

	// Apply the friction impulse vector
	glm::vec3 frictionImpulse = tangent * jt;

	if(invMassA > 0.0f && effectiveInvMassA > 0.0f && !aIsKinematic) {
		a.physicsBody->velocity -= frictionImpulse * invMassA;
	}
	if(invMassB > 0.0f && effectiveInvMassB > 0.0f && !bIsKinematic) {
		b.physicsBody->velocity += frictionImpulse * invMassB;
	}
}


void PhysicsSystem::PositionalCorrection(PhysicsProxy &a, PhysicsProxy &b, float penetration, const glm::vec3 &normal) {
	if(!IsSimulatable(a, b)) return;

	float invMassA = a.physicsBody->GetInverseMass();
	float invMassB = b.physicsBody->GetInverseMass();
	//float totalInvMass = invMassA + invMassB;

	//if(totalInvMass <= 0.0f) return;

	// Set slop to 0.0f temporarily to guarantee it pushes!
	const float percent = 1.0f; // Push 20% out per frame
	const float slop = 0.01f;

	float correctionAmount = std::max(penetration - slop, 0.0f) * percent;

	// If a tile-grounded body would be pushed further into the tile (downward),
	// treat it as immovable in that direction and give all correction to the other body.
	// normal.y < 0 means B receives a downward correction; normal.y > 0 means A does.
	float effectiveInvMassA = WouldPushIntoTile(a.physicsBody, -normal) ? 0.0f : invMassA;
	float effectiveInvMassB = WouldPushIntoTile(b.physicsBody, normal) ? 0.0f : invMassB;

	float totalInvMass = effectiveInvMassA + effectiveInvMassB;
	if(totalInvMass <= 0.0f)
		return;

	glm::vec3 correctionVector = normal * (correctionAmount / totalInvMass);

	bool aIsKinematic = a.physicsBody->isKinematic;
	bool bIsKinematic = b.physicsBody->isKinematic;

	if(invMassA > 0.0f && effectiveInvMassA > 0.0f && !aIsKinematic) {
		glm::vec3 currentPosA = a.transform->GetPosition();
		a.transform->SetPosition(currentPosA - (correctionVector * effectiveInvMassA));
	}
	if(invMassB > 0.0f && effectiveInvMassB > 0.0f && !bIsKinematic) {
		glm::vec3 currentPosB = b.transform->GetPosition();
		b.transform->SetPosition(currentPosB + (correctionVector * effectiveInvMassB));
	}
}

void PhysicsSystem::DetectTriggers(std::vector<Entity *> &) {
	for(size_t i = 0; i < physicsProxies.size(); ++i) {
		PhysicsProxy &triggerProxy = physicsProxies[i];

		// Use the Trigger component for trigger detection (separate from physics Collider)
		if(!triggerProxy.trigger) {
			continue;
		}

		if(!triggerProxy.trigger->IsEnabled()) {
			continue;
		}

		// Check against all other colliders
		for(size_t j = 0; j < physicsProxies.size(); ++j) {
			if(i == j) continue;

			PhysicsProxy &otherProxy = physicsProxies[j];
			if(!otherProxy.collider) continue;

			bool isColliding = triggerProxy.trigger->Intersects(
				*otherProxy.collider,
				triggerProxy.transform->GetPosition(),
				otherProxy.transform->GetPosition()
			);

			// Get the trigger state for this pair
			auto &triggersForEntity = triggerStates[triggerProxy.entity];
			bool wasCollidingLastFrame = triggersForEntity.find(otherProxy.entity) != triggersForEntity.end();

			if(isColliding && !wasCollidingLastFrame) {
				Events::TriggerEventArgs enterArgs{
					otherProxy.entity,
					Events::TriggerEventArgs::TriggerEventType::OnTriggerEnter
				};
				triggerProxy.entity->DispatchEntityEvent(Events::TriggerEventID::OnTriggerEnter, enterArgs);
				triggersForEntity.insert(otherProxy.entity);
			}
			else if(isColliding && wasCollidingLastFrame) {
				Events::TriggerEventArgs stayArgs{
					otherProxy.entity,
					Events::TriggerEventArgs::TriggerEventType::OnTriggerStay
				};
				triggerProxy.entity->DispatchEntityEvent(Events::TriggerEventID::OnTriggerStay, stayArgs);
			}
			else if(!isColliding && wasCollidingLastFrame) {
				Events::TriggerEventArgs exitArgs{
					otherProxy.entity,
					Events::TriggerEventArgs::TriggerEventType::OnTriggerExit
				};
				triggerProxy.entity->DispatchEntityEvent(Events::TriggerEventID::OnTriggerExit, exitArgs);
				triggersForEntity.erase(otherProxy.entity);
			}
		}
	}
}

void PhysicsSystem::RegisterEntity(Entity *entity) {
	PhysicsProxy proxy;
	proxy.entity = entity;
	proxy.transform = entity->Get<Components::Transform>();
	proxy.collider = entity->Get<Components::Collider>();
	proxy.physicsBody = entity->Get<Components::PhysicsBody>();
	proxy.trigger = entity->Get<Components::Trigger>();

	if(proxy.transform && proxy.collider) {
		physicsProxies.push_back(proxy);
	}
}

void PhysicsSystem::UnregisterEntity(Entity *entity) {
	physicsProxies.erase(
		std::remove_if(physicsProxies.begin(), physicsProxies.end(),
			[entity] (const PhysicsProxy &proxy) { return proxy.entity == entity; }),
		physicsProxies.end()
	);

	triggerStates.erase(entity);

	for(auto &triggerPair : triggerStates) {
		triggerPair.second.erase(entity);
	}
}

void PhysicsSystem::RegisterTileMap(Components::TileMap *tileMap) {
	if(tileMap) {
		tileMaps.push_back(tileMap);
	}
}

void PhysicsSystem::UnregisterTileMap(Components::TileMap *tileMap) {
	tileMaps.erase(
		std::remove(tileMaps.begin(), tileMaps.end(), tileMap),
		tileMaps.end()
	);
}
bool PhysicsSystem::DestroyTileAtWorldPos(float worldX, float worldY) {
	static const std::unordered_set<unsigned> empty{};
	return DestroyTileAtWorldPos(worldX, worldY, empty);
}

bool PhysicsSystem::DestroyTileAtWorldPos(float worldX, float worldY,
	const std::unordered_set<unsigned> &allowedTypes) {

	bool destroyed = false;
	for(auto *tileMap : tileMaps) {
		const TileMapData *data = tileMap->GetTileMapData();
		if(!data) continue;

		glm::vec3 entityPos(0.0f);
		glm::vec3 entityScale(1.0f);
		if(auto *parent = tileMap->Parent()) {
			if(auto *t = parent->Get<Components::Transform>()) {
				entityPos = t->GetPosition();
				entityScale = t->GetLocalScale();
			}
		}

		float localX = (worldX - entityPos.x) / entityScale.x;
		float localY = (worldY - entityPos.y) / entityScale.y;

		unsigned row = 0, col = 0;
		data->WorldToTile(localX, localY, row, col);

		if(row >= data->GetRows() || col >= data->GetCols()) continue;
		const unsigned tileType = data->GetTileAt(row, col);
		if(tileType == 0) continue;  // already empty
		// If allowedTypes is non-empty, the tile type must be in the set.
		if(!allowedTypes.empty() && allowedTypes.find(tileType) == allowedTypes.end()) continue;

		tileMap->NotifyTileDestroyed(row, col);
		destroyed = true;
	}
	return destroyed;
}

const std::string_view &PhysicsSystem::NameClass() const {
	static constexpr std::string_view className = NAMEOF(RassEngine::Systems::PhysicsSystem);
	return className;
}
}
