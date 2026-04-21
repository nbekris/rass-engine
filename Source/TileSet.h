// File Name:    TileSet.h
// Author(s):    main Boyuan Hu
// Course:       GAM541
// Project:      RASS
// Purpose:      Tile set resource management.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <string>

namespace RassEngine {

class Stream;

// TileSet defines tile properties and atlas configuration
class TileSet {
public:
	TileSet();
	~TileSet();

	// Load from JSON stream
	void Read(Stream &stream);

	// Getters
	const std::string &GetName() const {
		return name;
	}
	const std::string &GetTexturePath() const {
		return texturePath;
	}
	unsigned GetTileWidth() const {
		return tileWidth;
	}
	unsigned GetTileHeight() const {
		return tileHeight;
	}
	bool IsUsingAtlas() const {
		return useSpriteAtlas;
	}
	unsigned GetAtlasColumns() const {
		return spriteAtlasColumns;
	}
	unsigned GetAtlasRows() const {
		return spriteAtlasRows;
	}

private:
	std::string name;
	std::string texturePath;  // Direct texture path (like Sprite)
	unsigned tileWidth;
	unsigned tileHeight;
	bool useSpriteAtlas;
	unsigned spriteAtlasColumns;
	unsigned spriteAtlasRows;
};

} // namespace RassEngine
