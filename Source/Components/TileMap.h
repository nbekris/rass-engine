// File Name:    TileMap.h
// Author(s):    main Boyuan Hu, secondary Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Component rendering a tile-based map.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <string>
#include <string_view>

#include "Component.h"
#include "Cloneable.h"
#include "Events/GlobalEventArgs.h"
#include "Events/GlobalEventListener.h"

// Forward declarations
namespace RassEngine {
class Stream;
class TileMapData;
namespace Graphics {
class Mesh;
class Texture;
}
}

namespace RassEngine::Components {

// TileMap component for rendering tile-based maps
class TileMap : public Cloneable<Component, TileMap> {
public:
	TileMap();
	TileMap(const TileMap &other);
	virtual ~TileMap() override;

	// Inherited via Component
	virtual bool Initialize() override;
	virtual const std::string_view &NameClass() const override;
	virtual bool Read(Stream &stream) override;

	// Setters
	void SetTileMapDataName(const std::string &name) {
		tileMapDataName = name;
	}
	// Getters
	const std::string &GetTileMapDataName() const {
		return tileMapDataName;
	}
	const TileMapData *GetTileMapData() const {
		return tileMapData;
	}

	/// Marks tile at (row, col) as destroyed:
	///   - sets tileData[row*cols+col] = 0 (physics stops checking it)
	///   - does a single-texel glTexSubImage1D update (GPU state texture)
	void NotifyTileDestroyed(unsigned short row, unsigned short col);

	/// Reversibly opens or closes a door tile:
	///   - updates TileMapData::openDoorIndices (physics passability)
	///   - does a single-texel glTexSubImage1D update (GPU state texture)
	///   - does NOT modify tileData (original tile type is preserved)
	void SetTileDoorOpen(unsigned short row, unsigned short col, bool open);

private:
	// Render callback
	bool Render(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &);

	// Load resources (called in Initialize)
	void LoadResources();

	// Upload initial tile alive/dead states as a GL_R8UI 1D texture
	void InitTileStateTexture();
	// Member variables
	std::string tileMapDataName;  // Name of TileMapData file (e.g., "Level1")
	TileMapData *tileMapData = nullptr;
	Graphics::Mesh *mesh = nullptr;
	Graphics::Texture *texture = nullptr;
	bool filterLinear{false};
	bool usePhysicsCollider{false};
	//might be implemented in the future
	bool usePhysicsTrigger{false};
	int renderOrder{0};  // Lower values render first

	// GL handle for 1D tile-state texture (0 = not yet created / not a tilemap)
	uint32_t tileStateTexture = 0;
	// Cached tile size in world units, set during LoadResources
	float cachedTileSize = 1.0f;
	// Event listener
	Events::GlobalEventListener<TileMap> onRenderListener;
};

} // namespace RassEngine::Components
