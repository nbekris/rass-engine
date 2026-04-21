// File Name:    TileMapData.cpp
// Author(s):    main Boyuan Hu, secondary Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Data structure for tile map information.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "TileMapData.h"
#include "Stream.h"
#include "TileSet.h"
#include "Systems/Logging/ILoggingSystem.h"

namespace RassEngine {

TileMapData::TileMapData()
	: rows(0)
	, cols(0)
	, scale(1.0f)
	, mapBaseX(0.0f)
	, mapBaseY(0.0f)
	, tileSet(nullptr) {}

TileMapData::~TileMapData() {}

void TileMapData::Read(Stream &stream) {
	if(!stream.IsValid()) {
		return;
	}

	stream.PushNode("TileMapData");

	stream.Read("Name", name);
	stream.Read("Rows", rows);
	stream.Read("Cols", cols);
	stream.Read("Scale", scale);
	stream.Read("MapBaseX", mapBaseX);
	stream.Read("MapBaseY", mapBaseY);
	stream.Read("TileSet", tileSetName);  // Store name, load later

	// Read tile data
	if(stream.Contains("TileData")) {
		tileData.resize(rows * cols);
		stream.PushNode("TileData");

		stream.ReadArray("Tiles", [&, index = 0] () mutable {
			if(index < tileData.size()) {
				unsigned tileIndex;
				stream.Read("Index", tileIndex);
				tileData[index++] = tileIndex;
			}
			});

		stream.PopNode();
	}

	stream.PopNode();
}

unsigned TileMapData::GetTileAt(unsigned row, unsigned col) const {
	if(row >= rows || col >= cols) {
		return 0;
	}
	return tileData[row * cols + col];
}

void TileMapData::SetTileAt(unsigned row, unsigned col, unsigned tileIndex) {
	if(row >= rows || col >= cols) {
		return;
	}
	tileData[row * cols + col] = tileIndex;
}

void TileMapData::WorldToTile(float worldX, float worldY, unsigned &row, unsigned &col) const {
	if(!tileSet) {
		row = col = 0;
		return;
	}

	float localX = worldX - mapBaseX;
	float localY = worldY - mapBaseY;

	float tileWidth = tileSet->GetTileWidth() * scale;
	float tileHeight = tileSet->GetTileHeight() * scale;

	col = static_cast<unsigned>(localX / tileWidth);
	row = static_cast<unsigned>(localY / tileHeight);
}

void TileMapData::TileToWorld(unsigned row, unsigned col, float &worldX, float &worldY) const {
	if(!tileSet) {
		worldX = worldY = 0.0f;
		return;
	}

	float tileWidth = tileSet->GetTileWidth() * scale;
	float tileHeight = tileSet->GetTileHeight() * scale;

	worldX = mapBaseX + col * tileWidth;
	worldY = mapBaseY + row * tileHeight;
}

bool TileMapData::IsPassable(unsigned row, unsigned col) const {
	if(row >= rows || col >= cols) {
		return false;
	}
	const unsigned idx = row * cols + col;
	if(openDoorIndices.count(idx)) return true;  //door open, treat as passable
	return tileData[idx] == 0;
}

void TileMapData::SetDoorOpen(unsigned row, unsigned col, bool open) {
	if(row >= rows || col >= cols) return;
	const unsigned idx = row * cols + col;
	if(open) openDoorIndices.insert(idx);
	else      openDoorIndices.erase(idx);
}

bool TileMapData::IsDoorOpen(unsigned row, unsigned col) const {
	if(row >= rows || col >= cols) return false;
	return openDoorIndices.count(row * cols + col) > 0;
}

bool TileMapData::IsPassableAtWorldPos(float worldX, float worldY) const {
	if(!tileSet) {
		return true;
	}

	float localX = worldX - mapBaseX;
	float localY = worldY - mapBaseY;

	float tileWidth = tileSet->GetTileWidth() * scale;
	float tileHeight = tileSet->GetTileHeight() * scale;

	//if outside map bounds, consider it impassable
	if(localX < 0.0f || localY < 0.0f) {
		return true;
	}
	if(localX >= cols * tileWidth || localY >= rows * tileHeight) {
		return true;
	}

	unsigned col = static_cast<unsigned>(localX / tileWidth);
	unsigned row = static_cast<unsigned>(localY / tileHeight);

	return IsPassable(row, col);
}

int TileMapData::CheckAreaCollision(float worldX, float worldY, float width, float height) const {
	float halfWidth = width * 0.5f;
	float halfHeight = height * 0.5f;

	int collisionFlags = HP_NONE;

	//check corners of the area for collision
	if(!IsPassableAtWorldPos(worldX - halfWidth, worldY + halfHeight))
		collisionFlags |= HP_TOP_LEFT;

	if(!IsPassableAtWorldPos(worldX + halfWidth, worldY + halfHeight))
		collisionFlags |= HP_TOP_RIGHT;

	if(!IsPassableAtWorldPos(worldX - halfWidth, worldY - halfHeight))
		collisionFlags |= HP_BOTTOM_LEFT;

	if(!IsPassableAtWorldPos(worldX + halfWidth, worldY - halfHeight))
		collisionFlags |= HP_BOTTOM_RIGHT;

	return collisionFlags;
}
int TileMapData::GetCollisionCount(int flags) {
	int count = 0;
	if(flags & HP_TOP_LEFT) ++count;
	if(flags & HP_TOP_RIGHT) ++count;
	if(flags & HP_BOTTOM_LEFT) ++count;
	if(flags & HP_BOTTOM_RIGHT) ++count;
	return count;
}
} // namespace RassEngine
