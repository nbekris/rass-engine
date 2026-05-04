// File Name:    ResourceSystem.cpp
// Author(s):    main Taro Omiya, secondary Boyuan Hu, Niko Bekris, Eric Fleegal
// Course:       GAM541
// Project:      RASS
// Purpose:      System loading and caching game resources.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "ResourceSystem.h"
#include "Precompiled.h"
#include "Systems/Resource/ResourceSystem.h"
#include "Stream.h"
#include <memory>
#include <string>
#include <string_view>
#include <utility>

#include "Events/GlobalEventArgs.h"
#include "Events/SceneChange.h"
#include "Graphics/Mesh.h"
#include "Graphics/Texture.h"
#include "IEventListener.h"
#include "Systems/GlobalEvents/IGlobalEventsSystem.h"
#include "Systems/Logging/ILoggingSystem.h"
#include "Utils.h"

namespace RassEngine::Systems {

using namespace RassEngine::Graphics;

static constexpr std::string_view RESOURCES_FOLDER = "./Assets";
static constexpr char FOLDER_DIVIDER = '/';
static constexpr char EXTENSION_DIVIDER = '.';

ResourceSystem::ResourceSystem()
	: pathToTextureMap{}, quadMesh{nullptr}, textGridMeshMap{}, customMeshMap{}
	, onSceneShutdown{[this](const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &) {
		CleanUp();
		return true;
	}} {}

ResourceSystem::~ResourceSystem() {
	Shutdown();

	// Perform cleanup
	CleanUp();
}

bool ResourceSystem::Initialize() {
	auto* events = IGlobalEventsSystem::Get();
	if(events == nullptr) {
		LOG_ERROR("{} was unable to bind to {}", NameClass(), NAMEOF(Systems::IGlobalEventsSystem));
		return false;
	}

	// Bind to scene shutdown event
	events->bind(Events::SceneChange::AfterShutdown, &onSceneShutdown);
	return true;
}

void ResourceSystem::Shutdown() {
	auto *events = IGlobalEventsSystem::Get();
	if(events == nullptr) {
		return;
	}

	// Unbind to scene shutdown event
	events->unbind(Events::SceneChange::AfterShutdown, &onSceneShutdown);
}
std::string ResourceSystem::GenerateTileMapKey(
	const std::string &mapName,
	int width,
	int height
) const {
	return "TileMap_" + mapName + "_" + std::to_string(width) + "x" + std::to_string(height);
}

std::string ResourceSystem::GetFilePath(const std::string_view &relativeFilePath) const {
	// Append the resources folder path
	std::string toReturn{RESOURCES_FOLDER};
	if(relativeFilePath.empty()) {
		LOG_WARNING("{}: Empty path provided. Returning the root resources folder.", NameClass());
		return toReturn;
	}

	// Then check whether to append the folder divider
	if(!relativeFilePath.starts_with(FOLDER_DIVIDER)) {
		toReturn += FOLDER_DIVIDER;
	}

	// Append the file path
	toReturn += relativeFilePath;
	return toReturn;
}

std::string ResourceSystem::GetFilePath(const std::string_view &relativePath, const std::string_view &fileNameWithExtension) const {
	if(fileNameWithExtension.empty()) {
		LOG_WARNING("{}: Empty file name provided for path \"{}\". Returning path without file name.", NameClass(), relativePath);
		return GetFilePath(relativePath);
	}

	// Append the relative path
	std::string toReturn{relativePath};

	// Then check whether to append the folder divider
	if(!toReturn.ends_with(FOLDER_DIVIDER) && !fileNameWithExtension.starts_with(FOLDER_DIVIDER)) {
		toReturn += FOLDER_DIVIDER;
	}

	// Append the file name
	toReturn += fileNameWithExtension;

	// Apply the Resource folder prefix
	return GetFilePath(toReturn);
}

std::string ResourceSystem::GetFilePath(const std::string_view &relativePath, const std::string_view &fileName, const std::string_view &fileExtension) const {
	if(fileName.empty()) {
		LOG_WARNING("{}: Empty file name provided for path \"{}\". Returning path without file name.", NameClass(), relativePath);
		return GetFilePath(relativePath);
	} else if (fileExtension.empty()) {
		LOG_WARNING("{}: Empty file extension provided for file \"{}\". Returning path without extension.", NameClass(), fileName);
		return GetFilePath(relativePath, fileName);
	}

	// Append the file name
	std::string toReturn{fileName};

	// Check whether to append the extension divider
	if(!toReturn.ends_with(EXTENSION_DIVIDER) && !fileExtension.starts_with(EXTENSION_DIVIDER)) {
		toReturn += EXTENSION_DIVIDER;
	}

	// Append the file extension
	toReturn += fileExtension;

	// Apply the Resource folder prefix
	return GetFilePath(relativePath, toReturn);
}

Texture *ResourceSystem::GetTexture(const std::string_view &path, bool useLinear) {
	// Convert to string for map lookup, Cache pathkey with linear or nonelinear
	std::string cacheKey = std::string(path) + (useLinear ? ":L" : ":N");

	// Check if the texture has already been constructed
	auto it = pathToTextureMap.find(cacheKey);
	if(it != pathToTextureMap.end()) {
		return it->second.get();
	}

	// Construct a new texture
	std::unique_ptr<Texture> newTexture = std::make_unique<Texture>(std::string(path), useLinear);

	// Add to the map and return
	auto [inserted_it, success] = pathToTextureMap.emplace(cacheKey, std::move(newTexture));
	return inserted_it->second.get();
}

Graphics::Mesh *RassEngine::Systems::ResourceSystem::GetCustomMesh(const std::string_view &path) {
	// Check the cache
	std::string meshPath{path};
	auto it = customMeshMap.find(meshPath);
	if(it != customMeshMap.end()) {
		return it->second.get();
	}

	// build a new custom mesh
	auto mesh = std::make_unique<Graphics::Mesh>();
	mesh->BuildCustom(meshPath);

	// cache and return
	auto *meshPtr = mesh.get();
	customMeshMap[meshPath] = std::move(mesh);

	return meshPtr;
}

Graphics::Mesh *RassEngine::Systems::ResourceSystem::GetQuadMesh() {
	if(quadMesh == nullptr) {
		quadMesh = std::make_unique<Mesh>();
		quadMesh->BuildQuad();
	}
	return quadMesh.get();
}

Graphics::Mesh *ResourceSystem::GetTileMapMesh(
	const std::string &mapName,
	const std::vector<int> &tileIDs,
	int mapWidth,
	int mapHeight,
	float tileSize,
	int atlasColumns,
	int atlasRows
) {
	// Generate a unique key for this tile map configuration
	std::string key = GenerateTileMapKey(mapName, mapWidth, mapHeight);

	// Check the cache
	auto it = tileMapMeshMap.find(key);
	if(it != tileMapMeshMap.end()) {
		return it->second.get();
	}

	// build a new tile map mesh
	auto mesh = std::make_unique<Graphics::Mesh>();
	mesh->BuildTilemap(tileIDs, mapWidth, mapHeight, tileSize, atlasColumns, atlasRows);

	// cache and return
	auto *meshPtr = mesh.get();
	tileMapMeshMap[key] = std::move(mesh);

	return meshPtr;
}
Mesh *ResourceSystem::GetTextGridMesh(unsigned int maxCharacters) {
	// Retrieve a cached TextGridMesh, if available
	auto it = textGridMeshMap.find(maxCharacters);
	if(it != textGridMeshMap.end()) {
		return it->second.get();
	}

	// If not, create a new one
	std::unique_ptr<Mesh> textGridMesh = std::make_unique<Mesh>();
	textGridMesh->BuildTextGrid(maxCharacters, 0.1f, 0.1f);

	// Add to the map, and retrieve it from there
	// This is necessary because this function's scope
	// no longer own text textGridMesh
	auto [inserted_it, success] = textGridMeshMap.emplace(maxCharacters, std::move(textGridMesh));
	return inserted_it->second.get();
}
TileSet *ResourceSystem::GetTileSet(const std::string &name) {

	auto it = tileSetMap.find(name);
	if(it != tileSetMap.end()) {
		return it->second.get();
	}

	Stream stream{GetFilePath("Entities/TileSets/", name, "json")};

	if(!stream.IsValid()) {
		LOG_ERROR("Failed to load TileSet: {}", name);
		return nullptr;
	}

	auto tileSet = std::make_unique<TileSet>();
	tileSet->Read(stream);

	auto *ptr = tileSet.get();
	tileSetMap[name] = std::move(tileSet);
	return ptr;
}

TileMapData *ResourceSystem::GetTileMapData(const std::string &name) {

	auto it = tileMapDataMap.find(name);
	if(it != tileMapDataMap.end()) {
		return it->second.get();
	}

	Stream stream{GetFilePath("Entities/TileMaps", name, "json")};

	if(!stream.IsValid()) {
		LOG_ERROR("Failed to load TileMapData: {}", name);
		return nullptr;
	}

	auto tileMapData = std::make_unique<TileMapData>();
	tileMapData->Read(stream);

	const std::string &tileSetName = tileMapData->GetTileSetName();
	if(!tileSetName.empty()) {
		TileSet *tileSet = GetTileSet(tileSetName); 
		if(tileSet) {
			tileMapData->SetTileSet(tileSet);
		}
	}

	auto *ptr = tileMapData.get();
	tileMapDataMap[name] = std::move(tileMapData);
	return ptr;
}
const std::string_view &ResourceSystem::NameClass() const {
	static constexpr std::string_view className = NAMEOF(RassEngine::Systems::ResourceSystem);
	return className;
}

void ResourceSystem::CleanUp() {
	// Clear textures
	pathToTextureMap.clear();
	//quadMesh.reset();
	textGridMeshMap.clear();
	tileMapMeshMap.clear();
	tileMapDataMap.clear();
	tileSetMap.clear();
	// Clear quad mesh
	if(quadMesh != nullptr) {
		quadMesh = nullptr;
	}
}

}
