// File Name:    Sprite.cpp
// Author(s):    main Taro Omiya, secondary Boyuan Hu, Niko Bekris, Steven Yacoub, Eric Fleegal
// Course:       GAM541
// Project:      RASS
// Purpose:      Component rendering a 2D sprite image.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "Components/Sprite.h"

#include "Component.h"
#include "Components/Transform.h"
#include "Entity.h"
#include "Events/Global.h"
#include "Events/GlobalEventArgs.h"
#include "Graphics/Mesh.h"
#include "Graphics/texture.h"
#include "IEvent.h"
#include "Stream.h"
#include "Systems/GlobalEvents/IGlobalEventsSystem.h"
#include "Systems/Logging/ILoggingSystem.h"
#include "Systems/Render/IRenderSystem.h"
#include "Systems/Resource/IResourceSystem.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG

namespace RassEngine::Components {
using namespace RassEngine::Graphics;
using namespace RassEngine::Systems;

Sprite::Sprite()
	: Cloneable<Component, Sprite>{}
	, onRenderListener{this, &Sprite::Render} {}

Sprite::Sprite(const Sprite &other)
	: Cloneable<Component, Sprite>{other}
	, alpha{other.alpha}
	, color{other.color}
	, texturePath{other.texturePath}
	, renderLayer{other.renderLayer}
	, blendMode{other.blendMode}
	, numCols{other.numCols}
	, numRows{other.numRows}
	, frameindex{other.frameindex}
	, displayText{other.displayText}
	, fontGridCols{other.fontGridCols}
	, fontGridRows{other.fontGridRows}
	, filterLinear{other.filterLinear}
	, parallaxTiling{other.parallaxTiling}
	, cachedCharUVs{other.cachedCharUVs}
	, textDirty{other.textDirty}
	, onRenderListener{this, &Sprite::Render} {
	if(!texturePath.empty()) {
		if(auto *res = IResourceSystem::Get()) {
			textureHandle = res->AcquireTexture(texturePath, filterLinear);
		}
	}
}
	
Sprite::~Sprite() {
	// Release our texture reference.
	if(!texturePath.empty()) {
		if(auto *res = IResourceSystem::Get()) {
			res->ReleaseTexture(textureHandle);
		}
	}

	if(IGlobalEventsSystem::Get() == nullptr) {
		return;
	}
	IGlobalEventsSystem::Get()->unbind(Events::Global::Render, &onRenderListener);
}

bool Sprite::Initialize() {
	if(IGlobalEventsSystem::Get() == nullptr) {
		return false;
	}

	// Bind to the render function
	IGlobalEventsSystem::Get()->bind(Events::Global::Render, &onRenderListener);
	return true;
}

const std::string_view &Sprite::NameClass() const {
	static constexpr std::string_view className = NAMEOF(RassEngine::Components::Sprite);
	return className;
}

bool Sprite::Read(Stream &stream) {
	if(!Component::Read(stream)) {
		return false;
	}

	stream.Read("Alpha", alpha);
	stream.ReadVec3("Color", color);
	//stream.Read("TexturePath", texturePath);
	stream.Read("NumCols", numCols);
	stream.Read("NumRows", numRows);
	stream.Read("DisplayText", displayText);
	//stream.Read("FilterLinear", filterLinear);
	stream.ReadVec2("ParallaxTiling", parallaxTiling);
	std::string newPath;
	bool newLinear = filterLinear;
	stream.Read("TexturePath", newPath);
	stream.Read("FilterLinear", newLinear);
	RebindTexture(newPath, newLinear);  // acquires the texture
	// Reading RenderLayer
	auto readEnumLayer = static_cast<unsigned char>(IRenderSystem::RenderLayer::Transparent);
	if(stream.Read<unsigned char>("RenderLayer", readEnumLayer)) {
		renderLayer = static_cast<IRenderSystem::RenderLayer>(readEnumLayer);
	}
	std::string_view blendStr;
	if(stream.Read("BlendMode", blendStr)) {
		if(blendStr == "AlphaBlend")         blendMode = IRenderSystem::BlendMode::AlphaBlend;
		else if(blendStr == "Premultiplied") blendMode = IRenderSystem::BlendMode::Premultiplied;
		else if(blendStr == "Additive")      blendMode = IRenderSystem::BlendMode::Additive;
		else if(blendStr == "Multiply")      blendMode = IRenderSystem::BlendMode::Multiply;
		//else if(blendStr == "Screen")        blendMode = IRenderSystem::BlendMode::Screen;
		else LOG_WARNING("{}: Unrecognized BlendMode '{}'", NameClass(), blendStr);
	}
	return true;
}
void Sprite::SetColor(float r, float g, float b) {
	color = glm::vec3(r, g, b);
}
void Sprite::SetText(const std::string &text) {
	if(displayText == text) return;     // skip if no change
	displayText = text;
	textDirty = true;

	// Make sure text length is within limits
	if(displayText.length() > IRenderSystem::MAX_TEXT_LENGTH) {
		LOG_WARNING("Rendering text longer than {} characters currently not supported", IRenderSystem::MAX_TEXT_LENGTH);
	}
}

void Sprite::RebindTexture(const std::string &newPath, bool newLinear) {
	auto *res = IResourceSystem::Get();
	if(res == nullptr) {
		return;
	}
	if(textureHandle.IsValid() && newPath == texturePath && newLinear == filterLinear) {
		return;  // no change
	}
	// Acquire the new one before releasing the old (never drop the shared refcount to 0 mid-swap).
	TextureHandle newHandle = newPath.empty()
		? TextureHandle{}
	: res->AcquireTexture(newPath, newLinear);
	if(textureHandle.IsValid()) {
		res->ReleaseTexture(textureHandle);
	}
	texturePath = newPath;
	filterLinear = newLinear;
	textureHandle = newHandle;
}

void Sprite::SetTexture(const std::string &texturePath, bool filterLinear) {
	//this->texturePath = texturePath;
	//this->filterLinear = filterLinear;
	RebindTexture(texturePath, filterLinear);
}
void Sprite::SetOffset(float x, float y) {
	parallaxOffset = glm::vec2(x, y);
}
//(c - ' ')，#NNN# to  NNN cell
static std::vector<int> ParseRichText(std::string_view text) {
	std::vector<int> cells;
	cells.reserve(text.size());
	for(size_t i = 0; i < text.size();) {
		if(text[i] == '#') {
			// "##" to '#'
			if(i + 1 < text.size() && text[i + 1] == '#') {
				cells.push_back('#' - ' ');
				i += 2;
				continue;
			}
			// find the second '#'
			size_t close = text.find('#', i + 1);
			if(close != std::string_view::npos) {
				int cell = 0;
				auto begin = text.data() + i + 1;
				auto end = text.data() + close;
				auto [ptr, ec] = std::from_chars(begin, end, cell);
				if(ec == std::errc{} && ptr == end) {  // all characters are digits
					cells.push_back(cell);
					i = close + 1;
					continue;
				}
			}
			// Invalid: treat as a normal '#' to avoid swallowing characters
		}
		cells.push_back(static_cast<int>(text[i] - ' '));
		++i;
	}
	return cells;
}
bool Sprite::Render(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &) {
	// get transform component from parent entity
	Transform *trans = Parent()->Get<Transform>();
	if(!trans) return false;
	// set up model matrix
	if(!trans) {
		return false;
	}
	glm::mat4 modelTransform = trans->getTransformMatrix();

	// set up renderable
	IRenderSystem::Renderable renderable;
	renderable.modelMatrix = modelTransform;
	renderable.alpha = alpha;
	renderable.color = color;
	renderable.renderLayer = renderLayer;



	if(!displayText.empty()) {
		//Texture *fontTexture = IResourceSystem::Get()->GetTexture(texturePath, filterLinear);

		// Make sure text length is within limits
		if(displayText.length() > IRenderSystem::MAX_TEXT_LENGTH) {
			LOG_ERROR("Rendering text longer than {} characters currently not supported", IRenderSystem::MAX_TEXT_LENGTH);
			return false;
		}

		// get the shared TextGridMesh
		unsigned int meshSize = 32;  // default 32 characters

		// If the mesh size is shorter than the display text,
		// kepp doubling it
		while(meshSize < displayText.length()) {
			meshSize *= 2;
		}

		// Retrieve a cached version of this mesh size from resource system
		Mesh *textGridMesh = IResourceSystem::Get()->GetTextGridMesh(meshSize);

		//renderable.texture = fontTexture;
		renderable.texture = IResourceSystem::Get()->Resolve(textureHandle);
		renderable.mesh = textGridMesh;
		renderable.isTextMode = true;

		// Tiling = UV size occupied by a single character in the atlas
		renderable.texTiling = glm::vec2(
			1.0f / static_cast<float>(fontGridCols),
			1.0f / static_cast<float>(fontGridRows)
		);

		//calculate UV offsets for each character
		renderable.charUVOffsets.clear();
		renderable.charUVOffsets.reserve(displayText.length());
		//for(char c : displayText) {
		//	glm::vec2 uvOffset = CalculateCharUV(c);
		//	renderable.charUVOffsets.push_back(uvOffset);
		//}
		if(textDirty) {
			cachedCharUVs.clear();
			for(int cell : ParseRichText(displayText)) {   //parse rich text to get cell index, then calculate UV, only when text is dirty
				cachedCharUVs.push_back(CalculateCellUV(cell));
			}
			textDirty = false;
		}
		renderable.charUVOffsets = cachedCharUVs;
		renderable.charCount = static_cast<int>(cachedCharUVs.size());
	} else {
		//Texture *texture = IResourceSystem::Get()->GetTexture(texturePath, filterLinear);
		Mesh *mesh = IResourceSystem::Get()->GetQuadMesh();
		renderable.texture = IResourceSystem::Get()->Resolve(textureHandle);
		renderable.mesh = mesh;
		renderable.blendMode = blendMode;
		//
		//renderable.priorityOffset = trans->GetPosition().b;
		renderable.texTiling = glm::vec2(1.0f / numCols, 1.0f / numRows);
		renderable.texOffset = CalculateTexCoords(frameindex); // currently only supports single frame

		if(renderLayer == IRenderSystem::RenderLayer::Background) {
			renderable.texTiling = parallaxTiling;
			renderable.texOffset = parallaxOffset;
		}
	}

	IRenderSystem::Get()->SubmitRenderable(renderable);
	return true;
}

glm::vec2 Sprite::CalculateTexCoords(int frameIndex) {
	float u = (frameIndex % numCols) / static_cast<float>(numCols);
	float v = (frameIndex / numCols) / static_cast<float>(numRows);
	// Flip v coordinate because textures are typically upside down in OpenGL
	v = 1.0f - v - (1.0f / numRows);
	return glm::vec2(u, v);
}

glm::vec2 Sprite::CalculateCharUV(char c) {
	//int charIndex = static_cast<int>(c) - fontFirstASCII;
	int charIndex = static_cast<int>(c - ' ');
	if(charIndex < 0 || charIndex >= fontGridCols * fontGridRows) {
		charIndex = 0;  // Fallback to first character (usually space)
	}

	float u = (charIndex % fontGridCols) / static_cast<float>(fontGridCols);
	float v = (charIndex / fontGridCols) / static_cast<float>(fontGridRows);
	// Flip v coordinate
	v = 1.0f - v - (1.0f / fontGridRows);
	return glm::vec2(u, v);
}

glm::vec2 Sprite::CalculateCellUV(unsigned index) {
	if(index < 0 || index >= fontGridCols * fontGridRows) {
		index = 0;  // Fallback to first character (usually space)
	}
	float u = (index % fontGridCols) / static_cast<float>(fontGridCols);
	float v = (index / fontGridCols) / static_cast<float>(fontGridRows);
	// Flip v coordinate
	v = 1.0f - v - (1.0f / fontGridRows);
	return glm::vec2(u, v);
}
}
