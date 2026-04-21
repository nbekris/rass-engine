// File Name:    TileDoor.cpp
// Author(s):    main Boyuan Hu, secondary Niko Bekris, Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Component for a tile-based door controlled by switches.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "pch.h"
#include "TileDoor.h"

#include <Cloneable.h>
#include <Component.h>
#include <Components/Collider.h>
#include <Components/PhysicsBody.h>
#include <Components/TileMap.h>
#include <Components/Transform.h>
#include <TileMapData.h>
#include <Entity.h>
#include <TileSet.h>
#include <Events/Global.h>
#include <Events/GlobalEventArgs.h>
#include <IEvent.h>
#include <Stream.h>
#include <Systems/GlobalEvents/IGlobalEventsSystem.h>
#include <Systems/Logging/ILoggingSystem.h>
#include <Systems/Scene/ISceneSystem.h>
#include <Utils.h>
#include <UUID.h>

using namespace RassEngine;
using namespace RassEngine::Systems;

namespace RassGame::Components {

TileDoor::TileDoor()
	: Cloneable<Component, TileDoor>{}
	, doors{}, tileMap{nullptr}
	, onSwitchChanged{this, &TileDoor::OnSwitchChanged}
	, initialSyncDone{false}
	, onFirstUpdate{this, &TileDoor::OnFirstUpdate}
{}

TileDoor::TileDoor(const TileDoor &other)
	: Cloneable<Component, TileDoor>{other}
	, doors{other.doors}, tileMap{nullptr}
	, onSwitchChanged{this, &TileDoor::OnSwitchChanged}
	, initialSyncDone{false}	// clone always needs its own sync
	, onFirstUpdate{this, &TileDoor::OnFirstUpdate}
{
	// cachedSwitch pointers belong to the original entity's scene, they must be reset and re-resolved by Initialize()
	for(auto &entry : doors) {
		entry.cachedSwitch = nullptr;
	}
}

TileDoor::~TileDoor() {
	if(IGlobalEventsSystem::Get()) {
		IGlobalEventsSystem::Get()->unbind(Events::Global::Update, &onFirstUpdate);
	}
	for(auto &entry : doors) {
		if(entry.cachedSwitch) {
			entry.cachedSwitch->Unbind(&onSwitchChanged);
		}
	}
}

bool TileDoor::Initialize() {
	if(Parent() == nullptr) {
		LOG_WARNING("{}: Entity not found", NameClass());
		return false;
	}

	tileMap = Parent()->Get<RassEngine::Components::TileMap>();
	if(!tileMap) {
		LOG_WARNING("{}: TileMap component not found on same entity", NameClass());
		return false;
	}

	for(auto &entry : doors) {
		if(static_cast<UUID::Type>(entry.switchId) == 0u) continue;

		Switch *sw = FindSwitch(entry.switchId);
		if(!sw) continue;

		entry.cachedSwitch = sw;
		// SetTileDoorOpen deferred to OnFirstUpdate: TileMap's GPU texture
		// (tileStateTexture) is not yet ready at this point because
		// component order is alphabetical and TileDoor < TileMap.
		sw->Bind(&onSwitchChanged);
	}

	if(IGlobalEventsSystem::Get()) {
		IGlobalEventsSystem::Get()->bind(Events::Global::Update, &onFirstUpdate);
	}

	return true;
}

const std::string_view &TileDoor::NameClass() const {
	static constexpr std::string_view className = NAMEOF(Components::TileDoor);
	return className;
}

bool TileDoor::Read(Stream &stream) {
	if(!Component::Read(stream)) {
		return false;
	}

	doors.clear();
	stream.ReadArray("Doors", [this, &stream] () {
		DoorEntry entry;
		UUID::Type bindId{0u};
		stream.Read("BindTo", bindId);
		entry.switchId = UUID{bindId};

		stream.ReadArray("Tiles", [&entry, &stream] () {
			TileData newTile{0, 0, false};
			stream.Read("Row", newTile.row);
			stream.Read("Col", newTile.column);
			stream.Read("Open", newTile.isOpenByDefault);
			entry.tiles.emplace_back(newTile);
		});

		if(entry.tiles.empty()) {
			LOG_WARNING("{}: Door entry (BindTo={}) has no tile positions", NameClass(), bindId);
		}
		doors.push_back(std::move(entry));
	});

	if(doors.empty()) {
		LOG_WARNING("{}: No door entries specified", NameClass());
	}
	return true;
}

//reckless code, Just to avoid timing sequencec bug
bool TileDoor::OnFirstUpdate(
	const RassEngine::IEvent<RassEngine::Events::GlobalEventArgs> *,
	const RassEngine::Events::GlobalEventArgs &) {
	// Use flag instead of unbind to avoid modifying the event listener
	// list while EventSynchronous::call is iterating over it.
	if(initialSyncDone) return true;
	initialSyncDone = true;

	for(auto &entry : doors) {
		if(!entry.cachedSwitch) continue;
		for(auto &tile : entry.tiles) {
			tileMap->SetTileDoorOpen(tile.row, tile.column,
				entry.cachedSwitch->IsOn() != tile.isOpenByDefault);
		}
	}
	return true;
}

bool TileDoor::OnSwitchChanged(
	const RassEngine::IEvent<Switch::ToggleEventArgs> *,
	const Switch::ToggleEventArgs &args) {
	if(!tileMap) return true;

	const Switch *source = &args.source;

	for(auto &entry : doors) {
		if(entry.cachedSwitch != source) {
			continue;
		}
		for(auto &tile : entry.tiles) {
			const bool willBeOpen = source->IsOn() != tile.isOpenByDefault;
			if(!willBeOpen) {
				EjectEntitiesFromTile(tile.row, tile.column);
			}
			tileMap->SetTileDoorOpen(tile.row, tile.column, willBeOpen);
		}
	}
	return true;
}

void TileDoor::EjectEntitiesFromTile(unsigned short row, unsigned short col) {
	if(!tileMap) return;
	const TileMapData *mapData = tileMap->GetTileMapData();
	if(!mapData || !mapData->GetTileSet()) return;
	if(!ISceneSystem::Get()) return;

	// TileMapData coordinates are local to the tilemap's parent transform (matches
	// PhysicsSystem::CheckTileCollisions convention). Convert to world space.
	glm::vec3 tileMapWorldPos(0.0f);
	glm::vec3 tileMapScale(1.0f);
	if(auto *parent = tileMap->Parent()) {
		if(auto *parentTransform = parent->Get<RassEngine::Components::Transform>()) {
			tileMapWorldPos = parentTransform->GetPosition();
			tileMapScale = parentTransform->GetLocalScale();
		}
	}

	const float localTileW = static_cast<float>(mapData->GetTileSet()->GetTileWidth())  * mapData->GetScale();
	const float localTileH = static_cast<float>(mapData->GetTileSet()->GetTileHeight()) * mapData->GetScale();
	// Use absolute values: the tilemap may have a flipped (negative) scale for Y,
	// but collider AABB dimensions must be positive or GetBoxBounds produces an
	// inverted box (min > max) and every overlap test fails.
	const float tileWidth  = localTileW * std::abs(tileMapScale.x);
	const float tileHeight = localTileH * std::abs(tileMapScale.y);

	auto tileCenterWorld = [&](unsigned r, unsigned c) {
		float lx, ly;
		mapData->TileToWorld(r, c, lx, ly);
		return glm::vec2(
			tileMapWorldPos.x + (lx + localTileW * 0.5f) * tileMapScale.x,
			tileMapWorldPos.y + (ly + localTileH * 0.5f) * tileMapScale.y);
	};

	const glm::vec2 closingCenter = tileCenterWorld(row, col);
	const float tileCenterX = closingCenter.x;
	const float tileCenterY = closingCenter.y;

	// Temporary collider representing the closing tile's footprint, in world space
	RassEngine::Components::Collider tileCollider;
	tileCollider.SetBox(glm::vec3(tileWidth, tileHeight, 1.0f));

	// Collect all non-trigger physics bodies for overlap tests against candidate tiles
	struct EntityData {
		const Entity *entity;
		RassEngine::Components::Transform *transform;
		RassEngine::Components::Collider *collider;
		glm::vec3 pos;
	};
	std::vector<EntityData> physicsEntities;
	ISceneSystem::Get()->ForEachActiveEntity([&](const Entity *entity) {
		auto *transform   = entity->Get<RassEngine::Components::Transform>();
		auto *collider    = entity->Get<RassEngine::Components::Collider>();
		auto *physicsBody = entity->Get<RassEngine::Components::PhysicsBody>();
		if(!transform || !collider || !physicsBody) return;
		if(collider->IsTrigger()) return;
		physicsEntities.push_back({entity, transform, collider, transform->GetPosition()});
	});

	for(const auto &ed : physicsEntities) {
		const glm::vec3 tileCenterPos(tileCenterX, tileCenterY, ed.pos.z);
		if(!ed.collider->Intersects(tileCollider, ed.pos, tileCenterPos)) continue;

		// Score each cardinal neighbour by how aligned it is with the entity's
		// offset from the tile center — the tile the entity is "leaning toward" wins.
		const float dx = ed.pos.x - tileCenterX;
		const float dy = ed.pos.y - tileCenterY;

		struct Candidate { int dr, dc; float score; };
		const Candidate candidates[4] = {
			{-1,  0, -dy},  // up    (positive when entity is above center)
			{ 1,  0,  dy},  // down  (positive when entity is below center)
			{ 0, -1, -dx},  // left  (positive when entity is left of center)
			{ 0,  1,  dx},  // right (positive when entity is right of center)
		};

		int bestDr = 0, bestDc = 0;
		float bestScore = -1.0e30f;
		bool found = false;
		for(const auto &c : candidates) {
			const int nr = static_cast<int>(row) + c.dr;
			const int nc = static_cast<int>(col) + c.dc;
			if(nr < 0 || nc < 0) continue;
			if(!mapData->IsPassable(static_cast<unsigned>(nr), static_cast<unsigned>(nc))) continue;

			// Reject candidate tiles already occupied by another physics entity
			const glm::vec2 candXY = tileCenterWorld(static_cast<unsigned>(nr), static_cast<unsigned>(nc));
			const glm::vec3 candCenter(candXY.x, candXY.y, ed.pos.z);
			bool occupied = false;
			for(const auto &other : physicsEntities) {
				if(other.entity == ed.entity) continue;
				if(other.collider->Intersects(tileCollider, other.pos, candCenter)) {
					occupied = true;
					break;
				}
			}
			if(occupied) continue;

			if(c.score > bestScore) {
				bestScore = c.score;
				bestDr = c.dr;
				bestDc = c.dc;
				found = true;
			}
		}

		if(!found) {
			LOG_WARNING("{}: No passable neighbour to eject '{}' from closing tile ({}, {})",
				NameClass(), ed.entity->Name(), row, col);
			continue;
		}

		// Teleport entity to the center of the chosen neighbour tile
		const glm::vec2 targetXY = tileCenterWorld(
			static_cast<unsigned>(static_cast<int>(row) + bestDr),
			static_cast<unsigned>(static_cast<int>(col) + bestDc));
		ed.transform->SetPosition(glm::vec3(targetXY.x, targetXY.y, ed.pos.z));

		// Zero velocity so the physics system doesn't carry it back into the tile
		auto *physicsBody = ed.entity->Get<RassEngine::Components::PhysicsBody>();
		physicsBody->SetVelocity(glm::vec3(0.0f));
		physicsBody->impulseVelocity = glm::vec3(0.0f);
	}
}

Switch *TileDoor::FindSwitch(const RassEngine::UUID &id) const {
	if(ISceneSystem::Get() == nullptr) {
		LOG_WARNING("{}: {} is not registered", NameClass(), NAMEOF(ISceneSystem));
		return nullptr;
	}
	const Entity *e = ISceneSystem::Get()->FindEntity(id);
	if(!e) {
		LOG_WARNING("{}: Entity with ID {} was not found", NameClass(), static_cast<UUID::Type>(id));
		return nullptr;
	}
	Switch *sw = e->Get<Switch>();
	if(!sw) {
		LOG_WARNING("{}: Entity with ID {} has no Switch component", NameClass(), static_cast<UUID::Type>(id));
	}
	return sw;
}

} // namespace RassGame::Components
