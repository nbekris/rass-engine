// File Name:    TileMapData.h
// Author(s):    main Boyuan Hu
// Course:       GAM541
// Project:      RASS
// Purpose:      Data structure for tile map information.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <string>
#include <unordered_set>
#include <vector>

namespace RassEngine {

class Stream;
class TileSet;

enum HotPoint {
	HP_NONE = 0,
	HP_TOP_LEFT = 1 << 0,
	HP_TOP_RIGHT = 1 << 1,
	HP_BOTTOM_LEFT = 1 << 2,
	HP_BOTTOM_RIGHT = 1 << 3
};

// TileMapData stores tile layout and provides collision queries
class TileMapData {
public:
	TileMapData();
	~TileMapData();

	// Load from JSON stream
	void Read(Stream &stream);

	// Tile access
	unsigned GetTileAt(unsigned row, unsigned col) const;
	void SetTileAt(unsigned row, unsigned col, unsigned tileIndex);

	// Getters
	const std::string &GetName() const {
		return name;
	}
	const std::string &GetTileSetName() const {
		return tileSetName;
	}
	unsigned GetRows() const {
		return rows;
	}
	unsigned GetCols() const {
		return cols;
	}
	float GetScale() const {
		return scale;
	}
	float GetBaseX() const {
		return mapBaseX;
	}
	float GetBaseY() const {
		return mapBaseY;
	}
	const std::vector<unsigned> &GetTileData() const {
		return tileData;
	}
	const TileSet *GetTileSet() const {
		return tileSet;
	}

	// Set TileSet (called by ResourceSystem after loading)
	void SetTileSet(const TileSet *ts) {
		tileSet = ts;
	}

	// Coordinate conversion
	void WorldToTile(float worldX, float worldY, unsigned &row, unsigned &col) const;
	void TileToWorld(unsigned row, unsigned col, float &worldX, float &worldY) const;

	// Door state (reversible, does not modify tileData)
	void SetDoorOpen(unsigned row, unsigned col, bool open);
	bool IsDoorOpen(unsigned row, unsigned col) const;

	// Collision detection
	bool IsPassable(unsigned row, unsigned col) const;
	bool IsPassableAtWorldPos(float worldX, float worldY) const;
	int CheckAreaCollision(float worldX, float worldY, float width, float height) const;
	static bool HasCollision(int flags) { return flags != HP_NONE; }
  static bool IsStanding(int flags) { return (flags & HP_BOTTOM_LEFT) || (flags & HP_BOTTOM_RIGHT); }
  static bool IsTouchingCeiling(int flags) { return (flags & HP_TOP_LEFT) || (flags & HP_TOP_RIGHT); }
  static bool IsTouchingLeftWall(int flags) { return (flags & HP_TOP_LEFT) || (flags & HP_BOTTOM_LEFT); }
  static bool IsTouchingRightWall(int flags) { return (flags & HP_TOP_RIGHT) || (flags & HP_BOTTOM_RIGHT); }
  static int GetCollisionCount(int flags);
private:
	std::string name;
	std::string tileSetName;  // Name of TileSet to load
	unsigned rows;
	unsigned cols;
	float scale;
	float mapBaseX;
	float mapBaseY;
	std::vector<unsigned> tileData;
	std::unordered_set<unsigned> openDoorIndices;
	const TileSet *tileSet;  // Set by ResourceSystem
};

} // namespace RassEngine
