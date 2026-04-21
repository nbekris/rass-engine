// File Name:    TileMap.cpp
// Author(s):    main Boyuan Hu, secondary Taro Omiya, Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Component rendering a tile-based map.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "Components/TileMap.h"

#include <glbinding/gl/gl.h>
#include <glm/gtc/matrix_transform.hpp> 
#include <string_view>
#include <vector>

#include "Cloneable.h"
#include "Component.h"
#include "Components/Transform.h"
#include "Entity.h"
#include "Events/Global.h"
#include "Events/GlobalEventArgs.h"
#include "IEvent.h"
#include "Stream.h"
#include "Systems/GlobalEvents/IGlobalEventsSystem.h"
#include "Systems/Logging/ILoggingSystem.h"
#include "Systems/Render/IRenderSystem.h"
#include "Systems/Resource/IResourceSystem.h"
#include "Systems/Physics/IPhysicsSystem.h"
#include "TileMapData.h"
#include "TileSet.h"

namespace RassEngine::Components {

using namespace RassEngine::Graphics;
using namespace RassEngine::Systems;

TileMap::TileMap()
	: Cloneable<Component, TileMap>{}
	, onRenderListener{this, &TileMap::Render}
{
}

TileMap::TileMap(const TileMap &other)
	: Cloneable<Component, TileMap>{other}
	, tileMapDataName{other.tileMapDataName}
	, tileMapData{nullptr}
	, mesh{nullptr}
	, texture{nullptr}
	, filterLinear{other.filterLinear}
	, usePhysicsCollider{other.usePhysicsCollider}
	, renderOrder{other.renderOrder}
	, onRenderListener{this, &TileMap::Render}
{
}

TileMap::~TileMap() {
	if(tileStateTexture != 0) {
		gl::GLuint handle = static_cast<gl::GLuint>(tileStateTexture);
		gl::glDeleteTextures(1, &handle);
		tileStateTexture = 0;
	}
	if(IGlobalEventsSystem::Get() == nullptr) {
		return;
	}
	if(usePhysicsCollider) {
		IPhysicsSystem::Get()->UnregisterTileMap(this);
	}
	IGlobalEventsSystem::Get()->unbind(Events::Global::Render, &onRenderListener);
}

bool TileMap::Initialize() {
	if(IGlobalEventsSystem::Get() == nullptr) {
		return false;
	}

	// Bind to render event (same as Sprite)
	IGlobalEventsSystem::Get()->bind(Events::Global::Render, &onRenderListener);

	// Load resources if name specified
	if(!tileMapDataName.empty()) {
		LoadResources();
	}
	if(usePhysicsCollider) {
		IPhysicsSystem::Get()->RegisterTileMap(this);
	}
	return true;
}

const std::string_view &TileMap::NameClass() const {
	static constexpr std::string_view className = NAMEOF(RassEngine::Components::TileMap);
	return className;
}

bool TileMap::Read(Stream &stream) {
	if(!Component::Read(stream)) {
		return false;
	}
	// Read TileMapData name from JSON (e.g., "Level1")
	stream.Read("TileMapData", tileMapDataName);
	stream.Read("FilterLinear", filterLinear);
	stream.Read("UsePhysicsCollider", usePhysicsCollider);
	stream.Read("RenderOrder", renderOrder);
	//stream.Read("UsePhysicsTrigger", usePhysicsTrigger);
	return true;
}

void TileMap::LoadResources() {
	auto *resourceSystem = IResourceSystem::Get();
	if(!resourceSystem) {
		LOG_ERROR("TileMap: ResourceSystem not available");
		return;
	}

	// Load TileMapData through ResourceSystem (similar to GetTexture)
	tileMapData = resourceSystem->GetTileMapData(tileMapDataName);
	if(!tileMapData) {
		LOG_ERROR("TileMap: Failed to load TileMapData '{}'", tileMapDataName);
		return;
	}
	

	// Get TileSet from TileMapData
	const auto *tileSet = tileMapData->GetTileSet();
	if(!tileSet) {
		LOG_ERROR("TileMap: TileMapData '{}' has no TileSet", tileMapDataName);
		return;
	}
	cachedTileSize = tileSet->GetTileWidth() * tileMapData->GetScale();
	// Convert tile data to int vector
	const auto &tileData = tileMapData->GetTileData();
	std::vector<int> tileIDsInt(tileData.begin(), tileData.end());

	// Get or create cached TileMap mesh
	mesh = resourceSystem->GetTileMapMesh(
		tileMapData->GetName(),
		tileIDsInt,
		static_cast<int>(tileMapData->GetCols()),
		static_cast<int>(tileMapData->GetRows()),
		tileSet->GetTileWidth() * tileMapData->GetScale(),
		static_cast<int>(tileSet->GetAtlasColumns()),
		static_cast<int>(tileSet->GetAtlasRows())
	);

	if(!mesh) {
		LOG_ERROR("TileMap: Failed to create mesh for '{}'", tileMapDataName);
		return;
	}

	// Get texture directly from TileSet (same as Sprite's texturePath)
	if(!tileSet->GetTexturePath().empty()) {
		texture = resourceSystem->GetTexture(IResourceSystem::Path(tileSet->GetTexturePath()), filterLinear);
	}

	if(!texture) {
		LOG_WARNING("TileMap: No texture found for '{}'", tileMapDataName);
	}

	// Upload initial tile alive/dead states to GPU
	InitTileStateTexture();
}
void TileMap::InitTileStateTexture() {
	if(!tileMapData) return;

	const unsigned total = tileMapData->GetRows() * tileMapData->GetCols();
	const auto &tiles = tileMapData->GetTileData();

	// 1 byte per tile: 255 = alive, 0 = destroyed/empty
	std::vector<uint8_t> states(total);
	for(unsigned i = 0; i < total; ++i) {
		states[i] = (tiles[i] != 0) ? 255u : 0u;
	}

	if(tileStateTexture != 0) {
		gl::GLuint handle = static_cast<gl::GLuint>(tileStateTexture);
		gl::glDeleteTextures(1, &handle);
		tileStateTexture = 0;
	}

	gl::GLuint handle = 0;
	gl::glGenTextures(1, &handle);
	tileStateTexture = static_cast<uint32_t>(handle);

	gl::glBindTexture(gl::GL_TEXTURE_1D, handle);
	gl::glTexParameteri(gl::GL_TEXTURE_1D, gl::GL_TEXTURE_MIN_FILTER, gl::GL_NEAREST);
	gl::glTexParameteri(gl::GL_TEXTURE_1D, gl::GL_TEXTURE_MAG_FILTER, gl::GL_NEAREST);
	gl::glTexParameteri(gl::GL_TEXTURE_1D, gl::GL_TEXTURE_WRAP_S, gl::GL_CLAMP_TO_EDGE);
	gl::glTexImage1D(
		gl::GL_TEXTURE_1D, 0,
		gl::GL_R8UI,                  // internal format: 1-channel unsigned int
		static_cast<gl::GLsizei>(total), 0,
		gl::GL_RED_INTEGER,           // format
		gl::GL_UNSIGNED_BYTE,         // type
		states.data()
	);
	gl::glBindTexture(gl::GL_TEXTURE_1D, 0);
}
void TileMap::NotifyTileDestroyed(unsigned short row, unsigned short col) {
	if(!tileMapData || tileStateTexture == 0) return;

	// Update logic/physics data
	tileMapData->SetTileAt(row, col, 0);

	// Partial GPU texture update — only 1 texel, very cheap
	const unsigned short idx = row * tileMapData->GetCols() + col;
	const uint8_t dead = 0u;

	gl::glBindTexture(gl::GL_TEXTURE_1D, static_cast<gl::GLuint>(tileStateTexture));
	gl::glTexSubImage1D(
		gl::GL_TEXTURE_1D, 0,
		static_cast<gl::GLint>(idx), 1,
		gl::GL_RED_INTEGER,
		gl::GL_UNSIGNED_BYTE,
		&dead
	);
	gl::glBindTexture(gl::GL_TEXTURE_1D, 0);
}

void TileMap::SetTileDoorOpen(unsigned short row, unsigned short col, bool open) {
	if(!tileMapData || tileStateTexture == 0) return;

	// Update physics passability (openDoorIndices); tileData is NOT modified
	tileMapData->SetDoorOpen(row, col, open);

	// open=invisible(0), closed=visible(255)
	const unsigned short idx = row * tileMapData->GetCols() + col;
	const uint8_t state = open ? 0u : (tileMapData->GetTileAt(row, col) != 0 ? 255u : 0u);

	gl::glBindTexture(gl::GL_TEXTURE_1D, static_cast<gl::GLuint>(tileStateTexture));
	gl::glTexSubImage1D(
		gl::GL_TEXTURE_1D, 0,
		static_cast<gl::GLint>(idx), 1,
		gl::GL_RED_INTEGER,
		gl::GL_UNSIGNED_BYTE,
		&state
	);
	gl::glBindTexture(gl::GL_TEXTURE_1D, 0);
}
bool TileMap::Render(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &) {
	// Early exit if resources not loaded
	if(!mesh || !texture || !tileMapData) {
		return false;
	}
	/*InitTileStateTexture();*/
	// Get transform (same as Sprite)
	Transform *trans = Parent()->Get<Transform>();
	if(!trans) {
		return false;
	}

	// Build model matrix
	glm::mat4 modelTransform = trans->getTransformMatrix();

	// Apply TileMap base position offset
	glm::mat4 offsetMatrix = glm::translate(
		glm::mat4(1.0f),
		glm::vec3(tileMapData->GetBaseX(), tileMapData->GetBaseY(), 0.0f)
	);
	modelTransform = modelTransform * offsetMatrix;

	// Create renderable (same pattern as Sprite)
	IRenderSystem::Renderable renderable;
	renderable.modelMatrix = modelTransform;
	renderable.texture = texture;
	renderable.mesh = mesh;
	renderable.alpha = 1.0f;
	renderable.color = glm::vec3(1.0f, 1.0f, 1.0f);
	renderable.renderLayer = IRenderSystem::RenderLayer::Transparent;
	renderable.priorityOffset = renderOrder;
	renderable.texOffset = glm::vec2(0.0f, 0.0f);
	renderable.texTiling = glm::vec2(1.0f, 1.0f);
	renderable.isTextMode = false;
	renderable.tileStateTexture = tileStateTexture;
	renderable.tileSize = cachedTileSize;
	renderable.tileMapCols = static_cast<int>(tileMapData->GetCols());
	// Submit to render system
	IRenderSystem::Get()->SubmitRenderable(renderable);

	return true;
}

} // namespace RassEngine::Components
