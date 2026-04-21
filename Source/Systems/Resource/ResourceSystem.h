// File Name:    ResourceSystem.h
// Author(s):    main Taro Omiya, secondary Boyuan Hu, Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      System loading and caching game resources.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <memory>
#include <string>
#include <string_view>

#include "IResourceSystem.h"
#include "Events/GlobalEventListener.h"
#include "TileSet.h"
#include "TileMapData.h"
// Forward declarations
namespace RassEngine::Graphics {
class Texture;
class Mesh;
}

namespace RassEngine::Systems {

class ResourceSystem : public IResourceSystem {
public:
	ResourceSystem();
	virtual ~ResourceSystem();

	// Inherited via IResourceSystem
	bool Initialize() override;
	void Shutdown() override;
	Graphics::Texture *GetTexture(const std::string_view &path, bool useLinear = false) override;
	Graphics::Mesh *GetQuadMesh() override;
	Graphics::Mesh *GetTextGridMesh(unsigned int maxCharacters) override;
	Graphics::Mesh *GetTileMapMesh(
		const std::string &mapName,
		const std::vector<int> &tileIDs,
		int mapWidth,
		int mapHeight,
		float tileSize,
		int atlasColumns,
		int atlasRows
	) override;
	TileSet *GetTileSet(const std::string &name) override;
	TileMapData *GetTileMapData(const std::string &name) override;
	const std::string_view &NameClass() const override;

protected:
	std::string GetFilePath(const std::string_view &relativeFilePath) const override;
	virtual std::string GetFilePath(const std::string_view &relativePath, const std::string_view &fileNameWithExtension) const override;
	std::string GetFilePath(const std::string_view &relativePath, const std::string_view &fileName, const std::string_view &fileExtension) const override;

private:
	void CleanUp();
	std::string GenerateTileMapKey(const std::string &mapName, int width, int height) const;
	std::unordered_map<std::string, std::unique_ptr<Graphics::Mesh>> tileMapMeshMap;

	std::unordered_map<std::string, std::unique_ptr<Graphics::Texture>> pathToTextureMap;
	std::unique_ptr<Graphics::Mesh> quadMesh;
	std::unordered_map<unsigned int, std::unique_ptr<Graphics::Mesh>> textGridMeshMap;
	std::unordered_map<std::string, std::unique_ptr<TileSet>> tileSetMap;
	std::unordered_map<std::string, std::unique_ptr<TileMapData>> tileMapDataMap; 

	Events::GlobalEventListenerLambda onSceneShutdown;
};

}
