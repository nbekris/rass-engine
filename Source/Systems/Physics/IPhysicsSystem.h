// File Name:    IPhysicsSystem.h
// Author(s):    main Niko Bekris, secondary Boyuan Hu
// Course:       GAM541
// Project:      RASS
// Purpose:      System simulating physics interactions between entities.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once
#include "../GlobalEvents/IGlobalEventsSystem.h"
#include "Events/GlobalEventArgs.h"
#include <unordered_set>
namespace RassEngine {
	class Entity;

}
namespace RassEngine::Components {
class TileMap;
}
namespace RassEngine::Systems {

class IPhysicsSystem : public IGlobalSystem<IPhysicsSystem> {
public:
	virtual ~IPhysicsSystem() = default;
	virtual bool Update(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &) = 0;
	virtual void RegisterEntity(Entity *entity) = 0;
	virtual void UnregisterEntity(Entity *entity) = 0;
	virtual void RegisterTileMap(Components::TileMap *tileMap) = 0;
	virtual void UnregisterTileMap(Components::TileMap *tileMap) = 0;
	virtual bool DestroyTileAtWorldPos(float worldX, float worldY) = 0;
	virtual bool DestroyTileAtWorldPos(float worldX, float worldY,
		const std::unordered_set<unsigned> &allowedTypes) = 0;
protected:
	IPhysicsSystem() = default;
};
}
