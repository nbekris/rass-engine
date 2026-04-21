// File Name:    PhysicsSystem.h
// Author(s):    main Niko Bekris, secondary Taro Omiya, Boyuan Hu
// Course:       GAM541
// Project:      RASS
// Purpose:      System simulating physics interactions between entities.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <string_view>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "Events/GlobalEventArgs.h"
#include "Events/GlobalEventListener.h"
#include "IEvent.h"
#include "IPhysicsSystem.h"
#include "Components/PhysicsBody.h"

// Forward declaration
namespace RassEngine {
class Entity;
class TileMapData;
}

// Forward declaration
namespace RassEngine::Components {
class Transform;
class Collider;
class Trigger;

class TileMap;
}

namespace RassEngine::Systems {

class PhysicsSystem : public IPhysicsSystem {

	struct PhysicsProxy {
		Entity *entity;
		Components::Transform *transform;
		Components::Collider *collider;
		Components::PhysicsBody *physicsBody;
		Components::Trigger *trigger{nullptr};
	};

public:
	PhysicsSystem();
	virtual ~PhysicsSystem();

	bool Update(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &) override;
	void RegisterEntity(Entity *entity) override;
	void UnregisterEntity(Entity *entity) override;

	// Add these overrides to satisfy the ISystem interface
	bool Initialize() override;
	void Shutdown() override;
	const std::string_view &NameClass() const override;
	//TileMap Register
	void RegisterTileMap(Components::TileMap *tileMap);
	void UnregisterTileMap(Components::TileMap *tileMap);

	bool DestroyTileAtWorldPos(float worldX, float worldY) override;
	bool DestroyTileAtWorldPos(float worldX, float worldY, const std::unordered_set<unsigned> &allowedTypes) override;
private:
	void IntegrateBodies(float dt, std::vector<Entity *> &entities);

	void DetectCollisions(std::vector<Entity *> &entities);
	void DetectTriggers(std::vector<Entity *> &entities);

	void CheckTileCollisions(float dt);
	void ResolveTileCollision(PhysicsProxy &proxy, const TileMapData *tileMapData, const glm::vec3 &tileMapWorldPos, const glm::vec3 &tileMapScale, float dt);
	void ResolveCollisions(PhysicsProxy &a, PhysicsProxy &b);

	float GetOverlap(float minA, float maxA, float minB, float maxB);

	// Applied Physics Functions
	void ApplyImpulse(PhysicsProxy &a, PhysicsProxy &b, const glm::vec3 &normal);
	void ApplyFrictionImpulse(PhysicsProxy &a, PhysicsProxy &b, const glm::vec3 &normal);
	void PositionalCorrection(PhysicsProxy &a, PhysicsProxy &b, float penetration, const glm::vec3 &normal);

	void CleanUp();

	/// <summary>
	/// Checks whether both proxies are non-null and enabled
	/// </summary>
	inline static bool IsSimulatable(const PhysicsProxy &a, const PhysicsProxy &b) {
		return a.physicsBody && b.physicsBody && a.physicsBody->IsEnabled() && b.physicsBody->IsEnabled();
	}

public:
	glm::vec3 gravity;

private:
	Events::GlobalEventListener<PhysicsSystem> updateListener;
	Events::GlobalEventListenerLambda onSceneShutdown;

	std::vector<PhysicsProxy> physicsProxies;
	std::vector<Components::TileMap *> tileMaps;

	// trigger state tracking: map of trigger entity to set of entities currently colliding with it
	std::unordered_map<Entity *, std::unordered_set<Entity *>> triggerStates;
};
}
