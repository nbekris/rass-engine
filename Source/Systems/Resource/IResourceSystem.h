// File Name:    IResourceSystem.h
// Copyright © 2026 DigiPen (USA) Corporation.
#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include "Systems/GlobalEvents/IGlobalSystem.h"

namespace RassEngine {
class TileMapData;
class TileSet;
}
namespace RassEngine::Graphics {
class Mesh;
class Texture;
}

namespace RassEngine::Systems {

// Type-tagged, index-based resource handle.
// The Tag parameter is a phantom type: it never appears in a member, it only
// makes TextureHandle and (future) MeshHandle INCOMPATIBLE types at compile time.
template <typename Tag>
struct ResourceHandle {
	static constexpr std::uint32_t kInvalid = 0xFFFFFFFFu;
	std::uint32_t index = kInvalid;   // slot index inside ResourceSystem
	std::uint32_t generation = 0; // generation count for this slot (to detect stale handles)	
	[[nodiscard]] bool IsValid() const noexcept {
		return index != kInvalid;
	}
	friend bool operator==(ResourceHandle a, ResourceHandle b) noexcept {
		return a.index == b.index && a.generation == b.generation;
	}
	friend bool operator!=(ResourceHandle a, ResourceHandle b) noexcept {
		return !(a == b);
	}
};

using TextureHandle = ResourceHandle<struct TextureTag>;

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

	// --- Handle-based, frame-sliced (async) texture API --------------------------
	// AcquireTexture: registers a slot, bumps refCount, returns a handle immediately.
	//                 The texture is decoded/uploaded later by PumpLoads() over frames.
	// Resolve:        returns the Texture* only when Ready, otherwise nullptr.
	// Release:        decrements refCount (Stage 1 does NOT unload; that is Stage 2).
	// Pin:            marks a slot to survive future eviction (Stage 2).
	virtual TextureHandle AcquireTexture(const std::string_view &path, bool useLinear = false) = 0;
	virtual Graphics::Texture *Resolve(TextureHandle handle) = 0;
	virtual void ReleaseTexture(TextureHandle handle) = 0;
	virtual void PinTexture(TextureHandle handle, bool pinned) = 0;

	// --- Mesh / tile resources (unchanged) --------------------------------------
	virtual Graphics::Mesh *GetCustomMesh(const std::string_view &path) = 0;
	virtual Graphics::Mesh *GetQuadMesh() = 0;
	virtual Graphics::Mesh *GetTextGridMesh(unsigned int maxCharacters) = 0;
	virtual Graphics::Mesh *GetTileMapMesh(
		const std::string &mapName, const std::vector<int> &tileIDs,
		int mapWidth, int mapHeight, float tileSize, int atlasColumns, int atlasRows) = 0;
	virtual TileSet *GetTileSet(const std::string &name) = 0;
	virtual TileMapData *GetTileMapData(const std::string &name) = 0;

protected:
	virtual std::string GetFilePath(const std::string_view &relativeFilePath) const = 0;
	virtual std::string GetFilePath(const std::string_view &relativePath, const std::string_view &fileNameWithExtension) const = 0;
	virtual std::string GetFilePath(const std::string_view &relativePath, const std::string_view &fileName, const std::string_view &fileExtension) const = 0;
};

}
