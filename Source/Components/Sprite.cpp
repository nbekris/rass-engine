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
	, numCols{other.numCols}
	, numRows{other.numRows}
	, frameindex{other.frameindex}
	, displayText{other.displayText}
	, fontGridCols{other.fontGridCols}
	, fontGridRows{other.fontGridRows}
	, filterLinear{other.filterLinear}
	, parallaxTiling{other.parallaxTiling}
	, onRenderListener{this, &Sprite::Render} {}

Sprite::~Sprite() {
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
	stream.Read("TexturePath", texturePath);
	stream.Read("NumCols", numCols);
	stream.Read("NumRows", numRows);
	stream.Read("DisplayText", displayText);
	stream.Read("FilterLinear", filterLinear);
	stream.ReadVec2("ParallaxTiling", parallaxTiling);

	// Reading RenderLayer
	auto readEnumLayer = static_cast<unsigned char>(IRenderSystem::RenderLayer::Transparent);
	if(stream.Read<unsigned char>("RenderLayer", readEnumLayer)) {
		renderLayer = static_cast<IRenderSystem::RenderLayer>(readEnumLayer);
	}

	return true;
}
void Sprite::SetColor(float r, float g, float b) {
	color = glm::vec3(r, g, b);
}
void Sprite::SetText(const std::string &text) {
	displayText = text;

	// Make sure text length is within limits
	if(displayText.length() > IRenderSystem::MAX_TEXT_LENGTH) {
		LOG_WARNING("Rendering text longer than {} characters currently not supported", IRenderSystem::MAX_TEXT_LENGTH);
	}
}
void Sprite::SetTexture(const std::string &texturePath, bool filterLinear) {
	this->texturePath = texturePath;
	this->filterLinear = filterLinear;
}
void Sprite::SetOffset(float x, float y) {
	parallaxOffset = glm::vec2(x, y);
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
		Texture *fontTexture = IResourceSystem::Get()->GetTexture(texturePath,filterLinear);

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

		renderable.texture = fontTexture;
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
		for(char c : displayText) {
			glm::vec2 uvOffset = CalculateCharUV(c);
			renderable.charUVOffsets.push_back(uvOffset);
		}
		renderable.charCount = static_cast<int>(displayText.length());
	}
	else {
		Texture *texture = IResourceSystem::Get()->GetTexture(texturePath, filterLinear);
		Mesh *mesh = IResourceSystem::Get()->GetQuadMesh();
		renderable.texture = texture;
		renderable.mesh = mesh;
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

}
