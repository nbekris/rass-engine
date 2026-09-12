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
#include "Events/Global.h"
#include "Events/GlobalEventArgs.h"
#include "Events/SceneChange.h"
#include "Graphics/Mesh.h"
#include "Graphics/Texture.h"
#include "IEventListener.h"
#include "Systems/Time/ITimeSystem.h"
#include "Systems/GlobalEvents/IGlobalEventsSystem.h"
#include "Systems/Logging/ILoggingSystem.h"
#include "Utils.h"

namespace RassEngine::Systems {

using namespace RassEngine::Graphics;
//using TextureHandle = IResourceSystem::TextureHandle;
static constexpr std::string_view RESOURCES_FOLDER = "./Assets";
static constexpr char FOLDER_DIVIDER = '/';
static constexpr char EXTENSION_DIVIDER = '.';

// Constructor: drop pathToTextureMap from the init list.
ResourceSystem::ResourceSystem()
	: quadMesh{nullptr}, textGridMeshMap{}, customMeshMap{}
	, updateListener{this, &ResourceSystem::Update}
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
	// Bind to update event
	events->bind(Events::Global::Update, &updateListener);
	return true;
}

void ResourceSystem::Shutdown() {
	auto *events = IGlobalEventsSystem::Get();
	if(events == nullptr) {
		return;
	}

	// Unbind to scene shutdown event
	events->unbind(Events::SceneChange::AfterShutdown, &onSceneShutdown);
	// Unbind to update event
	events->unbind(Events::Global::Update, &updateListener);
}
bool ResourceSystem::Update(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &) {
	const float dt = ITimeSystem::Get()->GetUnscaledDeltaTime();
	timeSinceLastEviction += dt;
	//avoid frequently load&unload jittering
	if(timeSinceLastEviction >= kEvictGracePeriod) {
		timeSinceLastEviction = 0.0f;
		PumpEvictions();
	}

	PumpLoads();
	return true;
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

std::string ResourceSystem::MakeTextureKey(const std::string_view &path, bool useLinear) const {
	// Same key scheme as before: distinguish linear vs. nearest filtering.
	return std::string(path) + (useLinear ? ":L" : ":N");
}

//Texture *ResourceSystem::GetTexture(const std::string_view &path, bool useLinear) {
//	// Temporary "borrow": looks up (or loads) without changing the reference count.
//	std::string cacheKey = MakeTextureKey(path, useLinear);
//
//	auto it = pathToTextureMap.find(cacheKey);
//	if(it != pathToTextureMap.end()) {
//		return it->second.resource.get();
//	}
//
//	auto newTexture = std::make_unique<Texture>(std::string(path), useLinear);
//	auto [inserted_it, success] =
//		pathToTextureMap.emplace(cacheKey, CachedResource<Texture>{std::move(newTexture), 0, false});
//	return inserted_it->second.resource.get();
//}
TextureHandle ResourceSystem::AcquireTexture(const std::string_view &path, bool useLinear) {
	std::string key = MakeTextureKey(path, useLinear);

	if(auto it = keyToSlot.find(key); it != keyToSlot.end()) {
		auto &slot = textureSlots[it->second];
		++slot.refCount;
		return TextureHandle{it->second, slot.generation};
	}

	// first try to get from freeSlots
	std::uint32_t idx;
	if(!freeSlots.empty()) {
		idx = freeSlots.back();
		freeSlots.pop_back();
		// Reuse slot: generation was incremented during unload, so just read it here
	} else {
		idx = static_cast<std::uint32_t>(textureSlots.size());
		textureSlots.emplace_back();
	}
	TextureSlot &slot = textureSlots[idx];
	slot.cacheKey = key;
	slot.path = std::string(path);
	slot.useLinear = useLinear;
	slot.texture = std::make_unique<Graphics::Texture>();
	slot.state = LoadState::Queued;
	slot.refCount = 1;
	slot.pinned = false;
	//textureSlots.push_back(std::move(slot));
	keyToSlot.emplace(std::move(key), idx);
	pendingQueue.push_back(idx);
	return TextureHandle{idx, slot.generation};
}

Graphics::Texture *ResourceSystem::Resolve(TextureHandle handle) {
	if(!handle.IsValid() || handle.index >= textureSlots.size()) {
		return nullptr;
	}
	TextureSlot &slot = textureSlots[handle.index];
	if(slot.generation != handle.generation) {
		return nullptr;
	}
	if(slot.state != LoadState::Ready) {
		return nullptr;   // still loading, or failed
	}
	return slot.texture.get();
}

void ResourceSystem::ReleaseTexture(TextureHandle handle) {
	if(!handle.IsValid() || handle.index >= textureSlots.size()) {
		return;
	}
	TextureSlot &slot = textureSlots[handle.index];
	if(slot.generation != handle.generation) {
		return;   //ignore old handle
	}
	if(slot.refCount <= 0) {
		LOG_WARNING("{}: ReleaseTexture underflow on \"{}\"", NameClass(), slot.cacheKey);
		return;
	}
	--slot.refCount;

	if(slot.refCount == 0 && !slot.pinned) {
		evictQueue.push_back(EvictEntry{handle.index, slot.generation});
	}
}

void ResourceSystem::PinTexture(TextureHandle handle, bool pinned) {
	if(!handle.IsValid() || handle.index >= textureSlots.size()) {
		return;
	}
	TextureSlot &slot = textureSlots[handle.index];
	if(slot.generation != handle.generation) {
		return;
	}
	slot.pinned = pinned;
}
/*
void ResourceSystem::UnloadTexture(
	std::unordered_map<std::string, CachedResource<Texture>>::iterator it) {
	// FUTURE async/eviction: instead of erasing here, enqueue it->first for deferred
	// eviction and return. A later pass re-checks (refCount==0 && !pinned) before the
	// real unload, so a texture re-Acquired while "pending eviction" is simply revived
	// (lazy validation) — no unload/reload churn.
	LOG_INFO("{}: Unloading texture \"{}\"", NameClass(), it->first);
	pathToTextureMap.erase(it);
}
*/
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


void ResourceSystem::PumpLoads() {
	constexpr int kMaxDecodePerFrame = 1;   // CPU decode budget (stbi_load is heavy)
	constexpr int kMaxUploadPerFrame = 2;   // GPU upload budget (main thread only)

	int decoded = 0, uploaded = 0;
	std::vector<std::uint32_t> stillPending;
	stillPending.reserve(pendingQueue.size());

	for(std::uint32_t idx : pendingQueue) {
		TextureSlot &slot = textureSlots[idx];

		const bool isPending =
			(slot.state == LoadState::Queued || slot.state == LoadState::DecodingDone);
		if(!isPending || slot.path.empty()) {
			continue;
		}

		if(slot.state == LoadState::Queued && decoded < kMaxDecodePerFrame) {
			bool ok = slot.texture->LoadCPU(slot.path);
			slot.state = ok ? LoadState::DecodingDone : LoadState::Failed;
			++decoded;
		}
		if(slot.state == LoadState::DecodingDone && uploaded < kMaxUploadPerFrame) {
			bool ok = slot.texture->IntegrateGPU(slot.useLinear);
			slot.state = ok ? LoadState::Ready : LoadState::Failed;
			++uploaded;
		}
		if(slot.state != LoadState::Ready && slot.state != LoadState::Failed) {
			stillPending.push_back(idx);
		}
	}
	pendingQueue.swap(stillPending);
}

void ResourceSystem::PumpEvictions() {
	constexpr int kMaxEvictionPerFrame = 5;
	int evicted = 0;
	while(!evictQueue.empty() && evicted < kMaxEvictionPerFrame) {
		EvictEntry e = evictQueue.front();
		evictQueue.pop_front();
		TextureSlot &slot = textureSlots[e.idx];

		if(slot.generation != e.generation) {
			continue;
		}

		if(slot.refCount > 0 || slot.pinned) {
			continue;
		}
		UnloadSlot(e.idx);
		++evicted;
	}
}

void ResourceSystem::UnloadSlot(std::uint32_t idx) {
	//LOG_WARNING("{}: Unloading texture slot {} (\"{}\")", NameClass(), idx, textureSlots[idx].cacheKey);
	TextureSlot &slot = textureSlots[idx];
	keyToSlot.erase(slot.cacheKey);       // make key no longer hit the old slot
	slot.texture.reset();                  // release GPU/CPU
	slot.state = LoadState::Free;
	slot.cacheKey.clear();
	slot.path.clear();
	++slot.generation;                     // key: invalidate all old handles immediately
	freeSlots.push_back(idx);
}

void ResourceSystem::CleanUp() {
	// Optional diagnostic: flag slots still referenced at scene shutdown.
	// (Do NOT force-unload them — they may be legitimately cross-scene or pinned.)
	for(std::uint32_t i = 0; i < textureSlots.size(); ++i) {
		const TextureSlot &s = textureSlots[i];
		if(s.refCount > 0 && !s.pinned) {
			LOG_WARNING("{}: Texture \"{}\" still has {} ref(s) at scene shutdown (possible cross-scene leak)",
				NameClass(), s.cacheKey, s.refCount);
		}
	}

	while(!evictQueue.empty()) {
		EvictEntry e = evictQueue.front();
		evictQueue.pop_front();

		TextureSlot &slot = textureSlots[e.idx];
		if(slot.generation != e.generation) continue;   // stale entry, already reused
		if(slot.refCount > 0 || slot.pinned)  continue;   // revived or pinned → keep
		UnloadSlot(e.idx);                                 // free now, no grace period
	}


	textGridMeshMap.clear();
	tileMapMeshMap.clear();
	tileMapDataMap.clear();
	tileSetMap.clear();
	if(quadMesh != nullptr) {
		quadMesh = nullptr;
	}
}

}
