// File Name:    RenderSystemOpenGl.h
// Author(s):    main Taro Omiya, secondary Boyuan Hu, Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      System rendering all entities using OpenGL.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <string_view>
#include <vector>

#include "IRenderSystem.h"
#include "../Camera/ICameraSystem.h"

// Forward declaration
struct GLFWwindow;

// Forward declaration
namespace RassEngine::Graphics {
class Mesh;
class Texture;
class Shader;
}

namespace RassEngine::Systems {

// Forward declaration
class SystemsManager;

/// <summary>
/// OpenGL implementation of <see cref="IRenderSystem"/>
/// </summary>
class RenderSystemOpenGl : public IRenderSystem {
public:
	RenderSystemOpenGl(GLFWwindow *window);
	virtual ~RenderSystemOpenGl();

	// Inherited via Object
	const std::string_view &NameClass() const override;

	// Inherited via ISystem
	bool Initialize() override;
	void Shutdown() override;

	// Inherited via IRenderSystem
	void SubmitRenderable(const Renderable &renderable) override;
	inline int getScreenWidth() const override {
		return width;
	}
	inline int getScreenHeight() const override {
		return height;
	}

	// Toggle between windowed and fullscreen using GLFW primary monitor
	void ToggleFullscreen() override;
	bool IsFullscreen() const override;

protected:
	// Helper methods
	virtual bool BeginRender() override;
	virtual bool DrawRenderables() override;
	virtual bool EndRender() override;
	virtual void drawMesh(Graphics::Mesh *mesh, Graphics::Texture *texture = nullptr) const;

private:
	// Remove the rest of the default functions
	RenderSystemOpenGl() = delete;
	RenderSystemOpenGl(const RenderSystemOpenGl &) = delete;
	RenderSystemOpenGl(RenderSystemOpenGl &&) noexcept = delete;
	RenderSystemOpenGl &operator=(const RenderSystemOpenGl &) = delete;
	RenderSystemOpenGl &operator=(RenderSystemOpenGl &&) noexcept = delete;

	// Helper methods
	void UpdateProjection();
	void BeginFrame();
	void EndFrame();

	// Member variables
	int projectionLoc, viewLoc, width, height;
	float aspectRatio;

	glm::mat4 projection;
	glm::mat4 view;

	GLFWwindow *window;

	std::unique_ptr<Graphics::Shader> shader;
	std::vector<Renderable> renderQueue;

	// Saved windowed geometry; populated once when first entering fullscreen.
	// Kept separate from width/height (current framebuffer size, overwritten every BeginRender).
	struct WindowedGeom {
		int x{100}, y{100}, w{1280}, h{720};
	} windowedGeom_;
#ifdef _DEBUG
	bool isFullscreen_{false};
#else
	bool isFullscreen_{true};
#endif // _DEBUG

	ICameraSystem *cameraSystem{nullptr};
};

}
