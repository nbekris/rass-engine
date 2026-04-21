// File Name:    TileSet.cpp
// Author(s):    main Boyuan Hu, secondary Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Tile set resource management.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "TileSet.h"
#include "Stream.h"
#include "Systems/Logging/ILoggingSystem.h"

namespace RassEngine {

TileSet::TileSet()
	: tileWidth(32)
	, tileHeight(32)
	, useSpriteAtlas(false)
	, spriteAtlasColumns(1)
	, spriteAtlasRows(1) {}

TileSet::~TileSet() {}

void TileSet::Read(Stream &stream) {
	if(!stream.IsValid()) {
		return;
	}

	stream.PushNode("TileSet");

	stream.Read("Name", name);
	stream.Read("TexturePath", texturePath);  // Direct texture path
	stream.Read("TileWidth", tileWidth);
	stream.Read("TileHeight", tileHeight);
	stream.Read("UseSpriteAtlas", useSpriteAtlas);
	stream.Read("SpriteAtlasColumns", spriteAtlasColumns);
	stream.Read("SpriteAtlasRows", spriteAtlasRows);

	stream.PopNode();
}

} // namespace RassEngine
