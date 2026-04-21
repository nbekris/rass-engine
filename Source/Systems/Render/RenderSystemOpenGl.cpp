// File Name:    RenderSystemOpenGl.cpp
// Author(s):    main Taro Omiya, secondary Boyuan Hu, Niko Bekris, Eric Fleegal
// Course:       GAM541
// Project:      RASS
// Purpose:      System rendering all entities using OpenGL.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "RenderSystemOpenGl.h"

#include <algorithm>
#include <glbinding/gl/bitfield.h>
#include <glbinding/gl/enum.h>
#include <glbinding/gl/functions.h>
#include <glbinding/gl/types.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

// For some reason, <gl/GL.h> won't compile unless it's below all the above includes
#include <gl/GL.h>

#include "Graphics/Mesh.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Systems/ImGui/IImGuiSystem.h"
#include "Systems/DebugDraw/IDebugDrawSystem.h"
#include "Systems/Resource/IResourceSystem.h"
#include "Utils.h"

using namespace RassEngine::Graphics;

namespace RassEngine::Systems {

//using namespace gl;

static constexpr std::string_view SHADER_FOLDER = "Shaders";
static constexpr std::string_view VERT_SHADER_FILENAME = "basic";
static constexpr std::string_view FRAG_SHADER_FILENAME = "basic";

static constexpr std::string_view VERT_SHADER_EXTENSION = "vert";
static constexpr std::string_view FRAG_SHADER_EXTENSION = "frag";

static const gl::GLchar *PROJ_LOCATION = "projection";
static const gl::GLchar *VIEW_LOCATION = "view";

RenderSystemOpenGl::RenderSystemOpenGl(GLFWwindow *window)
	: window{window}
	, shader{nullptr}
	, projection(1.0f)
	, view{-1}
	, projectionLoc {-1}
	, viewLoc{-1}
	, renderQueue{} {
	if(window == nullptr) {
		throw std::invalid_argument("window cannot be null");
	}

	// Retrieve the screen width and height
	glfwGetFramebufferSize(window, &width, &height);
	aspectRatio = static_cast<float>(width) / static_cast<float>(height);
	gl::glViewport(0, 0, width, height);
}

RenderSystemOpenGl::~RenderSystemOpenGl() {
	Shutdown();
}

bool RenderSystemOpenGl::Initialize() {
	// Create and compile shaders
	shader = std::make_unique<Shader>();
	std::string filePath = IResourceSystem::Path(SHADER_FOLDER, VERT_SHADER_FILENAME, VERT_SHADER_EXTENSION);
	shader->AddShader(filePath, gl::GL_VERTEX_SHADER);
	filePath = IResourceSystem::Path(SHADER_FOLDER, FRAG_SHADER_FILENAME, FRAG_SHADER_EXTENSION);
	shader->AddShader(filePath, gl::GL_FRAGMENT_SHADER);
	shader->LinkProgram();

	// Get uniform locations
	projectionLoc = gl::glGetUniformLocation(shader->GetProgramId(), PROJ_LOCATION);
	if(projectionLoc == -1) {
		std::cerr << "Warning: 'projection' uniform not found!" << std::endl;
		return false;
	}

	viewLoc = gl::glGetUniformLocation(shader->GetProgramId(), VIEW_LOCATION);
	if (viewLoc == -1) {
		std::cerr << "Warning: 'view' uniform not found!" << std::endl;
		return false;
	}

	cameraSystem = ICameraSystem::Get();

	// Initialize projection matrix with default orthographic projection
	projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);

	return true;
}

void RenderSystemOpenGl::Shutdown() {
	// Clean-up the shaders
	if(shader != nullptr) {
		gl::glDeleteProgram(shader->GetProgramId());
		shader = nullptr;
	}
}

void RenderSystemOpenGl::drawMesh(Mesh *mesh, Texture *texture) const {
	// Make sure there's a mesh to draw
	if(mesh == nullptr) {
		return;
	}

	// Setup texture
	if(texture != nullptr) {
		int loc = gl::glGetUniformLocation(shader->GetProgramId(), "diffuseTex");
		if(loc != -1) {
			texture->BindTexture(0, shader->GetProgramId(), "diffuseTex");
		}
		loc = gl::glGetUniformLocation(shader->GetProgramId(), "uTiling");
		if(loc != -1) {
			gl::glUniform2f(loc, 1.0f, 1.0f);
		}
	}

	// Draw mesh
	mesh->Render();
}

void RenderSystemOpenGl::UpdateProjection() {
	// Update viewport
	gl::glViewport(0, 0, width, height);

	// Calculate aspect ratio and update projection matrix
	aspectRatio = static_cast<float>(width) / static_cast<float>(height);
	projection = glm::ortho(-aspectRatio, aspectRatio, -1.0f, 1.0f, -1.0f, 1.0f);
	gl::glEnable(static_cast<gl::GLenum>(GL_BLEND));
	gl::glBlendFunc(static_cast<gl::GLenum>(GL_SRC_ALPHA), static_cast<gl::GLenum>(GL_ONE_MINUS_SRC_ALPHA));
	// gl::glBlendFuncSeparate(static_cast<gl::GLenum>(GL_SRC_ALPHA), static_cast<gl::GLenum>(GL_ONE_MINUS_SRC_ALPHA), static_cast<gl::GLenum>(GL_ONE), static_cast<gl::GLenum>(GL_ZERO));
	gl::glEnable(static_cast<gl::GLenum>(GL_DEPTH_TEST));
	gl::glDepthFunc(static_cast<gl::GLenum>(GL_LEQUAL));
}

void RenderSystemOpenGl::BeginFrame() {
	// Clear the screen
	gl::glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	gl::glClear(static_cast<gl::ClearBufferMask>(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	// Activate shader and set uniforms
	shader->UseShader();
	gl::glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	view = cameraSystem->GetViewMatrix();
	gl::glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
}

void RenderSystemOpenGl::EndFrame() {
	// Unbind shader (optional, but good practice)
	shader->UnuseShader();

	// Swap buffers
	glfwSwapBuffers(window);
}

bool RenderSystemOpenGl::BeginRender() {
	// If before render begins, setup the frame.
	glfwGetFramebufferSize(window, &width, &height);
	UpdateProjection();
	BeginFrame();

	auto *imguiSystem = IImGuiSystem::Get();
	if(imguiSystem && imguiSystem->IsReady()) {
		imguiSystem->BeginFrame();
	}
	return true;
}

bool RenderSystemOpenGl::DrawRenderables(){
	std::sort(renderQueue.begin(), renderQueue.end(), [] (const Renderable &a, const Renderable &b) {
		if(a.renderLayer != b.renderLayer) {
			return a.renderLayer < b.renderLayer;
		}
		//now using z(b) for deoth sort if the game is topdown, using y(g) for deoth sort makes more scene
		//if(a.RenderLayer == b.RenderLayer)
		{
			float depthA = a.modelMatrix[3].z;
			float depthB = b.modelMatrix[3].z;
			if(std::abs(depthA - depthB) > std::numeric_limits<float>::epsilon()) {
				// opaque: front to back (correct for performance)
				// transparent: back to front (correct blending)
				if(a.renderLayer == RenderLayer::Transparent) {
					return depthA < depthB;  // back to front
				}
				return depthA > depthB;  // front to back
			}
			return a.priorityOffset < b.priorityOffset;
		}
		return a.priorityOffset < b.priorityOffset;
		});
	static const glm::mat4 identityView = glm::mat4(1.0f);
	RenderLayer currentLayer = static_cast<RenderLayer>(-1);
	for(const auto &renderable : renderQueue) {
		if(renderable.renderLayer != currentLayer) {
			currentLayer = renderable.renderLayer;

			// no view matrix for background and UI layers
			if(currentLayer == RenderLayer::Background || currentLayer == RenderLayer::UI) {
				
				gl::glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(identityView));
			} else {
				
				gl::glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			}
		}

		if(renderable.renderLayer == RenderLayer::Transparent) {
			// transparent object: do not write to depth buffer,but still do depth test
			gl::glDepthMask(GL_FALSE);
		} else {
			// opaque object: normal depth writing
			gl::glDepthMask(GL_TRUE);
		}
		// Set model matrix uniform
		int loc = gl::glGetUniformLocation(shader->GetProgramId(), "modelTransform");
		if(loc != -1) {
			gl::glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(renderable.modelMatrix));
		}
		// Set color uniform
		loc = gl::glGetUniformLocation(shader->GetProgramId(), "uColorTint");
		if(loc != -1) {
			gl::glUniform3f(loc, renderable.color.r, renderable.color.g, renderable.color.b);
		}
		// Set alpha uniform
		loc = gl::glGetUniformLocation(shader->GetProgramId(), "uAlpha");
		if(loc != -1) {
			gl::glUniform1f(loc, renderable.alpha);
		}
		loc = gl::glGetUniformLocation(shader->GetProgramId(), "diffuseTex");
		if(loc != -1 && renderable.texture != nullptr) {
			renderable.texture->BindTexture(0, shader->GetProgramId(), "diffuseTex");
		}
		loc = gl::glGetUniformLocation(shader->GetProgramId(), "uTiling");
		if(loc != -1) {
			gl::glUniform2f(loc, renderable.texTiling.x, renderable.texTiling.y);
		}
		loc = gl::glGetUniformLocation(shader->GetProgramId(), "uOffset");
		if(loc != -1) {
			gl::glUniform2f(loc, renderable.texOffset.x, renderable.texOffset.y);
		}


		// ========== text mode ==========
		loc = gl::glGetUniformLocation(shader->GetProgramId(), "uIsTextMode");
		if(loc != -1) {
			gl::glUniform1i(loc, renderable.isTextMode ? 1 : 0);
		}

		if(renderable.isTextMode && renderable.charCount > 0) {
			// upload character count
			loc = gl::glGetUniformLocation(shader->GetProgramId(), "uCharCount");
			if(loc != -1) {
				gl::glUniform1i(loc, renderable.charCount);
			}

			// upload character UV offsets array (will be refactored because using too much mem)
			loc = gl::glGetUniformLocation(shader->GetProgramId(), "uCharUVOffsets");
			if(loc != -1 && !renderable.charUVOffsets.empty()) {
				gl::glUniform2fv(loc, renderable.charCount,
					glm::value_ptr(renderable.charUVOffsets[0]));
			}
		}
		// ========== destructible tilemap ==========
		const bool isTileMap = (renderable.tileStateTexture != 0);
		loc = gl::glGetUniformLocation(shader->GetProgramId(), "uIsTileMap");
		if(loc != -1) {
			gl::glUniform1i(loc, isTileMap ? 1 : 0);
		}
		if(isTileMap) {
			// Bind tile state texture to unit 1
			gl::glActiveTexture(gl::GL_TEXTURE1);
			gl::glBindTexture(static_cast<gl::GLenum>(GL_TEXTURE_1D), static_cast<gl::GLuint>(renderable.tileStateTexture));
			loc = gl::glGetUniformLocation(shader->GetProgramId(), "uTileStateMap");
			if(loc != -1) {
				gl::glUniform1i(loc, 1);
			}
			loc = gl::glGetUniformLocation(shader->GetProgramId(), "uTileSize");
			if(loc != -1) {
				gl::glUniform1f(loc, renderable.tileSize);
			}
			loc = gl::glGetUniformLocation(shader->GetProgramId(), "uMapCols");
			if(loc != -1) {
				gl::glUniform1i(loc, renderable.tileMapCols);
			}
			// Restore texture unit 0 for subsequent draws
			gl::glActiveTexture(gl::GL_TEXTURE0);
		}
		// Draw the mesh with its texture
		if(renderable.mesh != nullptr) {
			drawMesh(renderable.mesh);
		}
	}
	gl::glDepthMask(GL_TRUE);
	return true;
}

bool RenderSystemOpenGl::EndRender() {

	auto *debugSystem = IDebugDrawSystem::Get();
	if(debugSystem) {
		debugSystem->RenderDebugVisualization();
	}

	auto *imguiSystem = IImGuiSystem::Get();
	if(imguiSystem && imguiSystem->IsReady()) {
		imguiSystem->EndFrame();
	}

	// Clear the render queue after drawing
	renderQueue.clear();

	// End the frame
	EndFrame();
	return true;
}

void RenderSystemOpenGl::SubmitRenderable(const Renderable& renderable) {
	renderQueue.push_back(renderable);
}

const std::string_view &RenderSystemOpenGl::NameClass() const {
	static constexpr std::string_view className = NAMEOF(RenderSystemOpenGl);
	return className;
}

void RenderSystemOpenGl::ToggleFullscreen() {
	isFullscreen_ = !isFullscreen_;

	if(isFullscreen_) {
		// width/height still hold last-frame windowed framebuffer size —
		// save them before BeginRender overwrites them with fullscreen dims.
		glfwGetWindowPos(window, &windowedGeom_.x, &windowedGeom_.y);
		//windowedGeom_.w = width;
		//windowedGeom_.h = height;

		GLFWmonitor *monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode *mode = glfwGetVideoMode(monitor);
		glfwSetWindowMonitor(window, monitor, 0, 0,
			mode->width, mode->height, mode->refreshRate);
	} else {
		glfwSetWindowMonitor(window, nullptr,
			windowedGeom_.x, windowedGeom_.y,
			windowedGeom_.w, windowedGeom_.h, 0);
	}
}

bool RenderSystemOpenGl::IsFullscreen() const {
	return isFullscreen_;
}
}
