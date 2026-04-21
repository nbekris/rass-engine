// File Name:    CameraSystem.cpp
// Author(s):    main Niko Bekris, secondary Boyuan Hu, Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      System managing the game camera and viewport.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "CameraSystem.h"

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <stdexcept>
#include <string_view>
#include <utility>

#include "Components/Camera.h"
#include "Components/Transform.h"
#include "Events/FixedUpdate.h"
#include "Events/GlobalEventArgs.h"
#include "Events/SceneChange.h"
#include "IEvent.h"
#include "Systems/GlobalEvents/IGlobalEventsSystem.h"
#include "Systems/Input/IInputSystem.h"
#include "Systems/Logging/ILoggingSystem.h"
#include "Systems/Render/IRenderSystem.h"
#include "Systems/Time/ITimeSystem.h"
#include "Utils.h"

namespace RassEngine::Systems {
	CameraSystem::CameraSystem()
	: updateListener{this, &CameraSystem::Update}
	, onSceneShutdown{[this](const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &) {
		return true;
	}} {}

	CameraSystem::~CameraSystem() {
		Shutdown();
	}

	bool CameraSystem::Initialize() {
		auto *events = IGlobalEventsSystem::Get();
		if(events == nullptr) {
			LOG_WARNING("{}: Could not initialize IGlobal Events System", NameClass());
			return false;
		}
		// bind to the update event
		events->bind(Events::FixedUpdate::After, &updateListener);
		events->bind(Events::SceneChange::BeforeShutdown, &onSceneShutdown);

		inputSystem = IInputSystem::Get();

		return true;
	}

	void CameraSystem::Shutdown() {
		auto *events = IGlobalEventsSystem::Get();
		if(events != nullptr) {
			events->unbind(Events::FixedUpdate::After, &updateListener);
			events->unbind(Events::SceneChange::BeforeShutdown, &onSceneShutdown);
		}
	}

	const std::string_view &CameraSystem::NameClass() const {
		static constexpr std::string_view className = NAMEOF(CameraSystem);
		return className;
	}

	glm::mat4 CameraSystem::GetViewMatrix() const {
		if(camera) {
			// aimOffset is added here for rendering only; it does NOT affect the window calculation
			return glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(camera->zoom)), glm::vec3(-(camera->position + camera->aimOffset)));
		} else {
			return glm::translate(glm::mat4(1.0f), glm::vec3(-position, 0.0f));
		}
	}
	glm::vec2 CameraSystem::ViewportToWorld(const glm::vec2 &ndcPos) const {
		auto *renderSystem = IRenderSystem::Get();
		if(!renderSystem) {
			return glm::vec2(0.0f);
		}

		float screenWidth = static_cast<float>(renderSystem->getScreenWidth());
		float screenHeight = static_cast<float>(renderSystem->getScreenHeight());
		float aspectRatio = screenWidth / screenHeight;

		// same to DebugDrawSystem::WorldToScreen 
		glm::mat4 projection = glm::ortho(-aspectRatio, aspectRatio, -1.0f, 1.0f, -1.0f, 1.0f);
		glm::mat4 view = GetViewMatrix();

		//NDC → World
		glm::mat4 invVP = glm::inverse(projection * view);
		glm::vec4 worldPos4D = invVP * glm::vec4(ndcPos.x, ndcPos.y, 0.0f, 1.0f);

		return glm::vec2(worldPos4D) / worldPos4D.w;
	}

	glm::vec2 CameraSystem::WorldToScreenPixels(const glm::vec3 &worldPos) const {
		auto *renderSystem = IRenderSystem::Get();
		if(renderSystem == nullptr) {
			LOG_ERROR("{}: Unable to retrieve {}", NameClass(), NAMEOF(IRenderSystem));
			throw std::runtime_error("Failed to get IRenderSystem");
		}

		// === Convert NDC [-1, 1] to screen space [0, width/height] ===
		glm::vec2 toReturn = WorldToScreenViewport(worldPos);
		toReturn.x = (toReturn.x + 1.0f) * 0.5f * static_cast<float>(renderSystem->getScreenWidth());
		// Y axis inverted
		toReturn.y = (1.0f - toReturn.y) * 0.5f * static_cast<float>(renderSystem->getScreenHeight());
		return toReturn;
	}

	glm::vec2 CameraSystem::WorldToScreenViewport(const glm::vec3 &worldPos) const {
		auto *renderSystem = IRenderSystem::Get();
		if(renderSystem == nullptr) {
			LOG_ERROR("{}: Unable to retrieve {}", NameClass(), NAMEOF(IRenderSystem));
			throw std::runtime_error("Failed to get IRenderSystem");
		}

		// === Step 1: Get transformation matrices ===
		float aspectRatio = static_cast<float>(renderSystem->getScreenWidth()) / static_cast<float>(renderSystem->getScreenHeight());

		// Projection matrix (same as RenderSystem)
		glm::mat4 projection = glm::ortho(-aspectRatio, aspectRatio, -1.0f, 1.0f, -1.0f, 1.0f);

		// === Step 2: Transform world position through MVP pipeline ===
		// Convert 2D world position to 4D homogeneous coordinates
		// Apply view transformation (world space -> view space)
		glm::vec4 viewSpace = GetViewMatrix() * glm::vec4(worldPos.x, worldPos.y, worldPos.z, 1.0f);

		// Apply projection transformation (view space -> clip space)
		glm::vec4 clipSpace = projection * viewSpace;

		// Perspective divide to get NDC (normalized device coordinates)
		// For orthographic projection, w = 1.0, but it's good practice
		glm::vec3 ndc = glm::vec3(clipSpace) / clipSpace.w;
		return glm::vec2(ndc.x, ndc.y);
	}

	bool CameraSystem::Update(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &) {
		float dt = Systems::ITimeSystem::Get()->GetDeltaTimeSec();

		FollowCamera(dt);
		LerpAimOffset(dt);
#ifdef _DEBUG
		TestZoom(dt);
#endif // _DEBUG
		LerpZoom(dt);
		LerpWindowOffset(dt);
		return true;
	}

	void CameraSystem::FollowCamera(float dt) {
		if(camera == nullptr) {
			return;
		}

		auto targetTransform = camera->GetTransform();
		const glm::vec3 charPos = targetTransform->GetPosition();

		// Compute the four window edges in world space
		const float left = camera->position.x + camera->offset.x - camera->cameraWindowSize.x * 0.5f + camera->cameraWindowOffset.x;
		const float right = camera->position.x + camera->offset.x + camera->cameraWindowSize.x * 0.5f + camera->cameraWindowOffset.x;
		const float bottom = camera->position.y + camera->offset.y - camera->cameraWindowSize.y * 0.5f + camera->cameraWindowOffset.y;
		const float top = camera->position.y + camera->offset.y + camera->cameraWindowSize.y * 0.5f + camera->cameraWindowOffset.y;

		// Per-axis: how far the character has gone past the window edge.
		// If the character is inside on that axis, the delta is exactly 0.
		glm::vec3 delta(0.0f);

		if(charPos.x < left)
			delta.x = charPos.x - left;        // negative: camera moves left
		else if(charPos.x > right)
			delta.x = charPos.x - right;       // positive: camera moves right

		if(charPos.y < bottom)
			delta.y = charPos.y - bottom;      // negative: camera moves down
		else if(charPos.y > top)
			delta.y = charPos.y - top;         // positive: camera moves up

		if(camera->followMode == Components::Camera::CameraFollowMode::Lerp)
			camera->position += delta * camera->followSpeed * dt;
		else if(camera->followMode == Components::Camera::CameraFollowMode::Push)
			camera->position += delta; // no smoothing, directly push
	}
	std::pair<glm::vec3, glm::vec3> CameraSystem::GetCameraWindowPosSize() const {
    if(camera == nullptr) return {glm::vec3(0.0f), glm::vec3(0.0f)};
    return {camera->position + camera->offset + camera->cameraWindowOffset, camera->cameraWindowSize};
	}

	bool CameraSystem::IsInCameraWindow(const glm::vec3 &worldPos) const {
		if(camera == nullptr) {
			return true;
		}
		glm::vec3 camPos = camera->position;
		glm::vec3 offset = camera->offset;
		glm::vec3 windowSize = camera->cameraWindowSize;
		glm::vec3 windowOffset = camera->cameraWindowOffset;
		return worldPos.x >= camPos.x + offset.x - windowSize.x * 0.5f - windowOffset.x &&
			worldPos.x <= camPos.x + offset.x + windowSize.x * 0.5f - windowOffset.x &&
			worldPos.y >= camPos.y + offset.y - windowSize.y * 0.5f - windowOffset.y &&
			worldPos.y <= camPos.y + offset.y + windowSize.y * 0.5f - windowOffset.y;	
	}
	void CameraSystem::LerpAimOffset(float dt) {
		if(camera == nullptr) {
			return;
		}
		camera->aimOffset += (camera->targetAimOffset - camera->aimOffset) * camera->aimOffsetLerpSpeed * dt;
	}
	void CameraSystem::LerpWindowOffset(float dt) {
		if(camera == nullptr) {
			return;
		}
		camera->cameraWindowOffset += (camera->targetCameraWindowOffset - camera->cameraWindowOffset) * camera->cameraWindowOffsetLerpSpeed * dt;
	}
	void CameraSystem::SetCameraWindowOffset(const glm::vec3 &offset) {
		if(camera == nullptr) {
			return;
		}
		camera->targetCameraWindowOffset = offset * camera->windowOffsetRange;
	}
	void CameraSystem::LerpZoom(float dt) {
		if(camera == nullptr) {
			return;
		}

		// Lerp in log-space for perceptually uniform zoom speed
		// (linear lerp on zoom value feels asymmetric: zoom-out is slow, zoom-in is fast)
		const float epsilon = 1e-4f;
		glm::vec3 safeCurrentZoom = glm::max(camera->zoom, glm::vec3(epsilon));
		glm::vec3 safeTargetZoom = glm::max(camera->targetZoom, glm::vec3(epsilon));
		glm::vec3 logCurrent = glm::log(safeCurrentZoom);
		glm::vec3 logTarget = glm::log(safeTargetZoom);
		camera->zoom = glm::exp(logCurrent + (logTarget - logCurrent) * camera->zoomLerpSpeed * dt);
	}
	void CameraSystem::MoveCamera(float dt) {
		if(inputSystem->IsKeyDown(GLFW_KEY_A))
			position.x -= speed * dt;

		if(inputSystem->IsKeyDown(GLFW_KEY_D))
			position.x += speed * dt;

		if(inputSystem->IsKeyDown(GLFW_KEY_W))
			position.y += speed * dt;

		if(inputSystem->IsKeyDown(GLFW_KEY_S))
			position.y -= speed * dt;
	}
	void CameraSystem::TestZoom(float dt) {
		if(camera == nullptr) {
			return;
		}
		if(inputSystem->IsKeyDown(GLFW_KEY_I))
			camera->targetZoom *= 0.9f; // zoom in
		if(inputSystem->IsKeyDown(GLFW_KEY_O))
			camera->targetZoom /= 0.9f;
	}
	void CameraSystem::RegisterCamera(Components::Camera *cameraComponent) {
		camera = cameraComponent;
	}

	void CameraSystem::UnregisterCamera(Components::Camera *cameraComponent) {
		if(camera == cameraComponent) {
			camera = nullptr;
		}
	}

	Components::Camera *CameraSystem::GetActiveCamera() const {
		return camera;
	}
}
