// File Name:    TileCheckPoint.cpp
// Author(s):    main Boyuan Hu
// Course:       GAM541
// Project:      RASS
// Purpose:      Component marking a tile as a player checkpoint.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "pch.h"
#include "TileCheckPoint.h"

#include <Cloneable.h>
#include <Component.h>
#include <Components/Movement.h>
#include <Components/TileMap.h>
#include <Components/Transform.h>
#include <Entity.h>
#include <Events/Global.h>
#include <Events/GlobalEventArgs.h>
#include <glm/vec3.hpp>
#include <IEvent.h>
#include <Stream.h>
#include <Systems/GlobalEvents/IGlobalEventsSystem.h>
#include <Systems/Logging/ILoggingSystem.h>
#include <Systems/Scene/ISceneSystem.h>
#include <TileMapData.h>
#include <TileSet.h>
#include <Utils.h>
#include <UUID.h>

using namespace RassEngine;
using namespace RassEngine::Systems;

namespace RassGame::Components {

TileCheckPoint::TileCheckPoint()
	: Cloneable<Component, TileCheckPoint>{}
	, onUpdate{this, &TileCheckPoint::OnUpdate}
{}

TileCheckPoint::TileCheckPoint(const TileCheckPoint &other)
	: Cloneable<Component, TileCheckPoint>{other}
	, checkpointTileIndex{other.checkpointTileIndex}
	//, playerEntityId{other.playerEntityId}
	, playerEntityName{other.playerEntityName}
	, onUpdate{this, &TileCheckPoint::OnUpdate}
	, checkAreaHeight{other.checkAreaHeight}
{}

TileCheckPoint::~TileCheckPoint() {
	if(IGlobalEventsSystem::Get()) {
		IGlobalEventsSystem::Get()->unbind(Events::Global::Update, &onUpdate);
	}
}

bool TileCheckPoint::Initialize() {
	if(Parent() == nullptr) {
		LOG_WARNING("{}: Entity not found", NameClass());
		return false;
	}

	tileMap = Parent()->Get<RassEngine::Components::TileMap>();
	if(!tileMap) {
		LOG_WARNING("{}: TileMap component not found on same entity", NameClass());
		return false;
	}

	// Lazy-resolve in OnUpdate; scene may not be fully loaded here
	cachedPlayer = FindPlayer(playerEntityName);

	if(IGlobalEventsSystem::Get()) {
		IGlobalEventsSystem::Get()->bind(Events::Global::Update, &onUpdate);
	}
	return true;
}

const std::string_view &TileCheckPoint::NameClass() const {
	static constexpr std::string_view className = NAMEOF(Components::TileCheckPoint);
	return className;
}

bool TileCheckPoint::Read(Stream &stream) {
	if(!Component::Read(stream)) return false;
	stream.Read("CheckpointTileIndex", checkpointTileIndex);
	UUID::Type pid{0u};
	stream.Read("PlayerID", pid);
	//playerEntityId = UUID{pid};
	stream.Read("PlayerName", playerEntityName);
	return true;
}

bool TileCheckPoint::OnUpdate(
	const IEvent<Events::GlobalEventArgs> *,
	const Events::GlobalEventArgs &) {

	if(!cachedPlayer) {
		cachedPlayer = FindPlayer(playerEntityName);
		if(!cachedPlayer) return true;
	}

	const TileMapData *data = tileMap->GetTileMapData();
	if(!data) return true;

	// Mirror PhysicsSystem::CheckTileCollisions: apply entity Transform before WorldToTile
	glm::vec3 mapWorldPos(0.0f);
	glm::vec3 mapScale(1.0f);
	if(auto *parent = tileMap->Parent()) {
		if(auto *t = parent->Get<RassEngine::Components::Transform>()) {
			mapWorldPos = t->GetPosition();
			mapScale = t->GetLocalScale();
		}
	}

	auto *playerTransform = cachedPlayer->Get<RassEngine::Components::Transform>();
	if(!playerTransform) return true;

	const glm::vec3 playerWorldPos = playerTransform->GetPosition();
	const float localX = (playerWorldPos.x - mapWorldPos.x) / mapScale.x;
	const float localY = (playerWorldPos.y - mapWorldPos.y) / mapScale.y;

	unsigned row, col;
	data->WorldToTile(localX, localY, row, col);

	// Not a checkpoint tile
	//if(data->GetTileAt(row, col) != checkpointTileIndex) return true;
	
	// Check if the player is within the check area height
	bool withinCheckArea = false;
	unsigned rowOffset = 0;	
	for(int i = 0; i < checkAreaHeight; ++i) {
		if(data->GetTileAt(row - i, col) == checkpointTileIndex) {
			withinCheckArea = true;
			rowOffset = i;
			break;
		}
	}
	if(!withinCheckArea) return true;
	row -= rowOffset;
	// Still on the same tile as last update — no change needed
	if(row == lastCpRow && col == lastCpCol) return true;
	//use the opendoor logic to disable rendering of the checkpoint tile, and the bg-1 layer flag can be shown.
	tileMap->SetTileDoorOpen(row,col,true);

	lastCpRow = row;
	lastCpCol = col;

	// Respawn at the center of this exact tile (last-touched semantics)
	float tileLocalX, tileLocalY;
	data->TileToWorld(row, col, tileLocalX, tileLocalY);

	if(const TileSet *ts = data->GetTileSet()) {
		tileLocalX += static_cast<float>(ts->GetTileWidth()) * data->GetScale() * 0.5f;
		tileLocalY += static_cast<float>(ts->GetTileHeight()) * data->GetScale() * 0.5f;
	}

	const float spawnWorldX = tileLocalX * mapScale.x + mapWorldPos.x;
	const float spawnWorldY = tileLocalY * mapScale.y + mapWorldPos.y;

	// SRP: push result to Movement; TileCheckPoint stores no output state
	if(auto *movement = cachedPlayer->Get<RassEngine::Components::Movement>()) {
		movement->SetLastCheckpoint(tileMap->Parent(), row, col);
	}

	return true;
}

Entity *TileCheckPoint::FindPlayer(const std::string &name) const {
	if(!ISceneSystem::Get()) {
		LOG_WARNING("{}: {} is not registered", NameClass(), NAMEOF(ISceneSystem));
		return nullptr;
	}
	if(name.empty()) return nullptr;

	const Entity *e = ISceneSystem::Get()->FindEntity(name);
	if(!e) {
		LOG_WARNING("{}: Player entity with name {} was not found",
			NameClass(), name);
		return nullptr;
	}
	return const_cast<Entity *>(e);
}

} // namespace RassGame::Components
