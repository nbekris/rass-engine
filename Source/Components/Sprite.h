// File Name:    Sprite.h
// Author(s):    main Taro Omiya, secondary Boyuan Hu, Steven Yacoub, Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Component rendering a 2D sprite image.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <glm/glm.hpp>
#include <string>
#include <string_view>

#include "Component.h"
#include "Cloneable.h"
#include "Events/GlobalEventArgs.h"
#include "Events/GlobalEventListener.h"
#include "Systems/Render/IRenderSystem.h"

// forward declaration
namespace RassEngine {
class Stream;
}

namespace RassEngine::Components {

using namespace RassEngine::Systems;

class Sprite : public Cloneable<Component, Sprite> {
public:

	Sprite();
	Sprite(const Sprite& other);
	virtual ~Sprite() override;

	// Inherited via Object
	virtual bool Initialize() override;
	virtual const std::string_view &NameClass() const override;

	// Inherited via Component
	virtual bool Read(Stream& stream) override;	// will be implemented after serialization is done

	inline float Alpha() const {
		return alpha;
	}

	inline void Alpha(float alpha) {
		this->alpha = alpha;
	}

	inline unsigned NumCols() const {
		return numCols;
	}

	inline void SetCols(unsigned cols) {
		this->numCols = cols;
		this->fontGridCols = static_cast<int>(cols);
	}

	inline unsigned NumRows() const {
		return numRows;
	}

	inline void SetRows(unsigned rows) {
		this->numRows = rows;
		this->fontGridRows = static_cast<int>(rows);
	}

	inline void SetFrame(unsigned frameIndex) {
		this->frameindex = frameIndex;
	}

	inline glm::vec3 Color() const {
		return color;
	}

	inline void Color(const glm::vec3 &color) {
		this->color = color;
	}

	inline std::string TexturePath() const {
		return texturePath;
	}

	//temp setter for testing, will be replaced by deserialization
	inline void TexturePath(const std::string &path) {
		texturePath = path;
	}

	inline IRenderSystem::RenderLayer GetRenderLayer() const {
		return renderLayer;
	}

	inline void SetRenderLayer(IRenderSystem::RenderLayer layer) {
		this->renderLayer = layer;
	}

	void SetText(const std::string &text);
	void SetTexture(const std::string &texturePath, bool filterLinear = false);
	void SetColor(float r, float g, float b);
	void SetOffset(float x, float y);
private:
	bool Render(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &);
	glm::vec2 CalculateTexCoords(int frameIndex);
	glm::vec2 CalculateCharUV(char c);
	// Member variables
	float alpha{1.0f};
	IRenderSystem::RenderLayer renderLayer{IRenderSystem::RenderLayer::Transparent};
	glm::vec3 color{1.0f, 1.0f, 1.0f};

	std::string texturePath{};
	unsigned numCols{1};
	unsigned numRows{1};
	unsigned frameindex{0};
	bool filterLinear{false};  // false=GL_NEAREST,true=GL_LINEAR
	//Text rendering specific
	std::string displayText{};
	int fontGridCols = 16;
	int fontGridRows = 8;
	glm::vec2 parallaxOffset{0.0f};
	glm::vec2 parallaxTiling{1.0f, 1.0f};
	Events::GlobalEventListener<Sprite> onRenderListener;
};

}
