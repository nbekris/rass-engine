// File Name:    IResourceSystem.h
// Author(s):    main Taro Omiya, secondary Boyuan Hu, Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      System loading and caching game resources.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "Systems/GlobalEvents/IGlobalSystem.h"

// Forward declaration
namespace RassEngine {
class TileMapData;
class TileSet;
}

namespace RassEngine::Graphics {
class Mesh;
class Texture;
}

namespace RassEngine::Systems {

class IResourceSystem : public IGlobalSystem<IResourceSystem> {
public:
	inline static std::string Path(const std::string_view &relativeFilePath) {
		return Get()->GetFilePath(relativeFilePath);
	}
	inline static std::string Path(const std::string_view &relativePath, const std::string_view &fileNameWithExtension) {
		return Get()->GetFilePath(relativePath, fileNameWithExtension);
	}
	inline static std::string Path(const std::string_view &relativePath, const std::string_view &fileName, const std::string_view &fileExtension) {
		return Get()->GetFilePath(relativePath, fileName, fileExtension);
	}

	virtual Graphics::Texture *GetTexture(const std::string_view &path, bool useLinear = false) = 0;
	virtual Graphics::Mesh *GetCustomMesh(const std::string_view &path) = 0;
	virtual Graphics::Mesh *GetQuadMesh() = 0;
	virtual Graphics::Mesh *GetTextGridMesh(unsigned int maxCharacters) = 0;
	virtual Graphics::Mesh *GetTileMapMesh(
		const std::string &mapName,
		const std::vector<int> &tileIDs,
		int mapWidth,
		int mapHeight,
		float tileSize,
		int atlasColumns,
		int atlasRows
	) = 0;
	virtual TileSet *GetTileSet(const std::string &name) = 0;
	virtual TileMapData *GetTileMapData(const std::string &name) = 0;
protected:
	virtual std::string GetFilePath(const std::string_view &relativeFilePath) const = 0;
	virtual std::string GetFilePath(const std::string_view &relativePath, const std::string_view &fileNameWithExtension) const = 0;
	virtual std::string GetFilePath(const std::string_view &relativePath, const std::string_view &fileName, const std::string_view &fileExtension) const = 0;
};

}
