// File Name:    IRenderSystem.h
// Author(s):    main Taro Omiya, secondary Boyuan Hu, Steven Yacoub, Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      System rendering all entities using OpenGL.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "../GlobalEvents/IGlobalEventsSystem.h"

// Forward declaration
namespace RassEngine {
class Engine;
}

// Forward declaration
namespace RassEngine::Graphics {
class Mesh;
class Texture;
}

namespace RassEngine::Systems {

/// <summary>
/// System that's responsible for some rendering logic
/// </summary>
class IRenderSystem : public IGlobalSystem<IRenderSystem> {
	friend class Engine;
public:
	static constexpr unsigned MAX_TEXT_LENGTH = 128;
	enum class RenderLayer : unsigned char {
		Background = 0,
		Opaque,
		Transparent,
		UI
	};

	/// <summary>
	/// Packet of data to render
	/// </summary>
	struct Renderable {
		Graphics::Mesh *mesh{nullptr};
		Graphics::Texture *texture{nullptr};
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		glm::vec2 texOffset = glm::vec2(0.0f);
		glm::vec2 texTiling = glm::vec2(1.0f);
		glm::vec3 color = glm::vec3(1.0f);
		int priorityOffset = 0;
		float alpha = 1.0f;
		RenderLayer renderLayer = RenderLayer::Transparent;
		//Text rendering specific
		bool isTextMode = false;
		std::vector<glm::vec2> charUVOffsets{MAX_TEXT_LENGTH};
		int charCount = 0;
		uint32_t tileStateTexture = 0;  // GL handle for 1D R8UI tile-state texture
		float tileSize = 1.0f;          // World-space size of one tile (used in fragment shader)
		int tileMapCols = 0;            // Number of columns in the tile map
	};
public:
	virtual ~IRenderSystem() = default;

	/// <summary>
	/// Adds a renderable data into the render queue
	/// </summary>
	virtual void SubmitRenderable(const Renderable &renderable) = 0;
	/// <summary>
	/// Returns the screen width.
	/// </summary>
	virtual int getScreenWidth() const = 0;
	/// <summary>
	/// Returns the screen height.
	/// </summary>
	virtual int getScreenHeight() const = 0;

	/// <summary>
	/// Toggles between windowed and fullscreen mode (GLFW primary monitor).
	/// Stores and restores windowed position/size automatically.
	/// </summary>
	virtual void ToggleFullscreen() = 0;

	/// <summary>
	/// Returns true if currently in fullscreen mode.
	/// </summary>
	virtual bool IsFullscreen() const = 0;

protected:
	// Do not allow constructing an interface
	IRenderSystem() = default;

	// Engine-specific methods
	virtual bool BeginRender() = 0;
	virtual bool DrawRenderables() = 0;
	virtual bool EndRender() = 0;

private:
	// Remove the rest of the default functions
	IRenderSystem(const IRenderSystem &) = delete;
	IRenderSystem(IRenderSystem &&) noexcept = delete;
	IRenderSystem &operator=(const IRenderSystem &) = delete;
	IRenderSystem &operator=(IRenderSystem &&) noexcept = delete;
};

};
