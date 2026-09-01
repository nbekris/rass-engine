// File Name:    ResourceSystem.h
// Copyright © 2026 DigiPen (USA) Corporation.
#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <deque>
#include "IResourceSystem.h"
#include "Events/GlobalEventArgs.h"
#include "Events/GlobalEventListener.h"
#include "TileSet.h"
#include "TileMapData.h"

namespace RassEngine::Graphics {
class Texture;
class Mesh;
}

namespace RassEngine::Systems {

class ResourceSystem : public IResourceSystem {
public:
	ResourceSystem();
	virtual ~ResourceSystem();

	bool Initialize() override;
	void Shutdown() override;

	// Handle-based texture API
	TextureHandle AcquireTexture(const std::string_view &path, bool useLinear = false) override;
	Graphics::Texture *Resolve(TextureHandle handle) override;
	void ReleaseTexture(TextureHandle handle) override;
	void PinTexture(TextureHandle handle, bool pinned) override;

	// Mesh / tile resources (unchanged)
	Graphics::Mesh *GetCustomMesh(const std::string_view &path) override;
	Graphics::Mesh *GetQuadMesh() override;
	Graphics::Mesh *GetTextGridMesh(unsigned int maxCharacters) override;
	Graphics::Mesh *GetTileMapMesh(
		const std::string &mapName, const std::vector<int> &tileIDs,
		int mapWidth, int mapHeight, float tileSize, int atlasColumns, int atlasRows) override;
	TileSet *GetTileSet(const std::string &name) override;
	TileMapData *GetTileMapData(const std::string &name) override;
	const std::string_view &NameClass() const override;
	static constexpr float kEvictGracePeriod =5.0f;
protected:
	std::string GetFilePath(const std::string_view &relativeFilePath) const override;
	std::string GetFilePath(const std::string_view &relativePath, const std::string_view &fileNameWithExtension) const override;
	std::string GetFilePath(const std::string_view &relativePath, const std::string_view &fileName, const std::string_view &fileExtension) const override;

private:
	void CleanUp();
	std::string GenerateTileMapKey(const std::string &mapName, int width, int height) const;
	std::string MakeTextureKey(const std::string_view &path, bool useLinear) const;

	// Per-frame driver that advances queued texture loads (decode + GPU upload).
	bool Update(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &);
	void PumpLoads();
	void PumpEvictions();
	void UnloadSlot(std::uint32_t idx);
	// --- Async texture slots ----------------------------------------------------
	enum class LoadState {
		Free, Queued, DecodingDone, Ready, Failed
	};

	// A persistent slot. Stage 1: slots are append-only (never removed), so a
	// TextureHandle's index stays valid for the whole program lifetime.
	struct TextureSlot {
		std::string cacheKey;                          // path + linear/nearest suffix
		std::string path;                              // relative asset path
		bool useLinear = false;
		std::unique_ptr<Graphics::Texture> texture;
		LoadState state = LoadState::Queued;
		int refCount = 0;
		bool pinned = false;
		std::uint32_t generation = 0;
	};
	struct EvictEntry {
		std::uint32_t idx;
		std::uint32_t generation;   // generation while enqueued, to detect stale entries
	};
	std::vector<TextureSlot> textureSlots;             // index == TextureHandle.index
	std::unordered_map<std::string, std::uint32_t> keyToSlot;  // dedup by cacheKey
	std::vector<std::uint32_t> pendingQueue;           // slots awaiting Pump
	std::deque<EvictEntry> evictQueue;            // slots to evict (refCount == 0 && !pinned)
	std::vector<std::uint32_t> freeSlots;              // slots that can be reused
	// Mesh / tile caches (unchanged)
	std::unordered_map<std::string, std::unique_ptr<Graphics::Mesh>> tileMapMeshMap;
	std::unordered_map<std::string, std::unique_ptr<Graphics::Mesh>> customMeshMap;
	std::unique_ptr<Graphics::Mesh> quadMesh;
	std::unordered_map<unsigned int, std::unique_ptr<Graphics::Mesh>> textGridMeshMap;
	std::unordered_map<std::string, std::unique_ptr<TileSet>> tileSetMap;
	std::unordered_map<std::string, std::unique_ptr<TileMapData>> tileMapDataMap;
	float timeSinceLastEviction = 0.0f;
	Events::GlobalEventListenerLambda onSceneShutdown;
	Events::GlobalEventListener<ResourceSystem> updateListener;
};

}
