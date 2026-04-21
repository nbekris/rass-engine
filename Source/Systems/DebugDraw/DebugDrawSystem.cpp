// File Name:    DebugDrawSystem.cpp
// Author(s):    main Boyuan Hu, secondary Niko Bekris, Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      System rendering debug shapes and overlays.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "DebugDrawSystem.h"

#include "Entity.h"
#include "Scene.h"
#include "Components/Transform.h"
#include "Components/PhysicsBody.h"
#include "Components/Collider.h"
#include "Components/ShootingController.h"
#include "Components/HealthComponent.h"
#include "Systems/Scene/ISceneSystem.h"
#include "Systems/Time/ITimeSystem.h"
#include "Systems/Logging/ILoggingSystem.h"
#include "Systems/Render/IRenderSystem.h"
#include "Systems/Camera/ICameraSystem.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
namespace RassEngine::Systems {

DebugDrawSystem::DebugDrawSystem() {}

DebugDrawSystem::~DebugDrawSystem() {
	Shutdown();
}

bool DebugDrawSystem::Initialize() {
	LOG_INFO("DebugDrawSystem initialized");
	return true;
}

void DebugDrawSystem::Shutdown() {
	LOG_INFO("DebugDrawSystem shutdown");
}

const std::string_view &DebugDrawSystem::NameClass() const {
	static constexpr std::string_view className = NAMEOF(Systems::DebugDrawSystem);
	return className;
}

void DebugDrawSystem::RenderDebugVisualization() {
	if(!enableDebug_) return;
#ifdef _DEBUG

	// === 1. Render debug control panel ===
	RenderDebugPanel();

	// === 2. Render FPS overlay ===
	if(showFPS_) {
		RenderFPSOverlay();
	}
#endif
	// === 3. Draw debug info for all entities in the scene ===
	ImDrawList *drawList = ImGui::GetBackgroundDrawList();
	DrawAllEntitiesDebugInfo(drawList);
}

void DebugDrawSystem::RenderDebugPanel() {
	// Toggle panel visibility with F1 key
	if(ImGui::IsKeyPressed(ImGuiKey_F1)) {
		showDebugPanel_ = !showDebugPanel_;
	}

	if(!showDebugPanel_) return;

	ImGui::SetNextWindowPos(ImVec2(10, 50), ImGuiCond_FirstUseEver);
	ImGui::Begin("Debug Visualization", &showDebugPanel_, ImGuiWindowFlags_AlwaysAutoResize);

	ImGui::Text("Press F1 to toggle this panel");
	ImGui::Separator();

	ImGui::Checkbox("Enable Debug Drawing", &enableDebug_);
	ImGui::Separator();

	ImGui::Text("Visualization Options:");
	ImGui::Checkbox("Show Collider Boxes", &showColliderBoxes_);
	ImGui::Checkbox("Show Transform Gizmos", &showTransformGizmos_);
	ImGui::Checkbox("Show Entity Names", &showEntityNames_);
	ImGui::Checkbox("Show Speed Vectors", &showSpeedVectors_);
	ImGui::Checkbox("Show Mesh Wireframe", &showMeshWireframe_);
	ImGui::Checkbox("Show FPS Counter", &showFPS_);
	ImGui::Checkbox("Show Aim Directions", &showAimDirections_);
	ImGui::Checkbox("Show Camera Window", &showCameraWindow_);
	ImGui::Separator();

	if(ImGui::CollapsingHeader("Performance", ImGuiTreeNodeFlags_DefaultOpen)) {
		auto *timeSystem = ITimeSystem::Get();
		if(timeSystem) {
			float fps = timeSystem->GetCurrentFPS();
			float frameTime = timeSystem->GetDeltaTimeSec() * 1000.0f;

			ImGui::Text("FPS: %.1f", fps);
			ImGui::Text("Frame Time: %.3f ms", frameTime);

			// Update FPS history for graph
			fpsHistory_[fpsHistoryIndex_] = fps;
			fpsHistoryIndex_ = (fpsHistoryIndex_ + 1) % FPS_HISTORY_SIZE;

			ImGui::PlotLines("##FPS", fpsHistory_, FPS_HISTORY_SIZE, fpsHistoryIndex_,
				nullptr, 0.0f, 120.0f, ImVec2(0, 50));
		}
	}

	if(ImGui::CollapsingHeader("Scene Info")) {
		auto *sceneSystem = ISceneSystem::Get();
		if(sceneSystem) {
			ImGui::Text("Scene Running: %s", sceneSystem->IsRunning() ? "Yes" : "No");
			ImGui::Text("Scene Changing: %s", sceneSystem->IsSceneChanging() ? "Yes" : "No");
			ImGui::Text("Scene Restarting: %s", sceneSystem->IsSceneRestarting() ? "Yes" : "No");
		}
	}

	ImGui::End();
}

void DebugDrawSystem::RenderFPSOverlay() {
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
	ImGui::SetNextWindowBgAlpha(0.35f);
	ImGui::Begin("FPS Overlay", nullptr,
		ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoFocusOnAppearing |
		ImGuiWindowFlags_NoNav);

	auto *timeSystem = ITimeSystem::Get();
	if(timeSystem) {
		float fps = timeSystem->GetCurrentFPS();
		ImGui::Text("FPS: %.0f", fps);
		ImGui::Text("%.2f ms", 1000.0f / fps);
	}
	ImGui::End();
}

void DebugDrawSystem::DrawAllEntitiesDebugInfo(ImDrawList *drawList) {
	auto *sceneSystem = ISceneSystem::Get();
	if(!sceneSystem || !sceneSystem->IsRunning()) return;


	sceneSystem->ForEachActiveEntity([this, drawList] (const Entity *entity) {
		DrawEntityDebugInfo(entity, drawList);
	});


	/*
	Scene* currentScene = sceneSystem->GetCurrentScene();
	if (currentScene) {
		const auto& entities = currentScene->GetEntities();
		for (const auto& entity : entities) {
			if (entity && !entity->IsDestroyed()) {
				DrawEntityDebugInfo(entity.get(), drawList);
			}
		}
	}
	*/
}

void DebugDrawSystem::DrawEntityDebugInfo(const Entity *entity, ImDrawList *drawList) {
	if(!entity) return;


	if(showHealthBars_) {
		DrawHealthBar(entity, drawList);
	}
#ifdef _DEBUG
	if(showColliderBoxes_) {
		DrawColliderBox(entity, drawList);
	}

	if(showTransformGizmos_) {
		DrawTransformGizmo(entity, drawList);
	}

	if(showSpeedVectors_) {
		DrawSpeedVector(entity, drawList);
	}

	if(showEntityNames_) {
		DrawEntityName(entity, drawList);
	}

	if(showAimDirections_) {
		DrawAimDirection(entity, drawList);
	}

	if(showCameraWindow_) {
		DrawCameraWindow(entity, drawList);
	}
#endif // _DEBUG
}

void DebugDrawSystem::DrawAimDirection(const Entity *entity, ImDrawList *drawList) {
	auto *shooter = entity->Get<Components::ShootingController>();
	if(!shooter) return;

	auto *transform = entity->GetTransform();
	if(!transform) return;

	const glm::vec2 aimDir = shooter->GetAimDirection();
	if(glm::length(aimDir) < 1e-6f) return;

	glm::vec2 worldPos(transform->GetPosition());

	constexpr float visualLength = 1.5f;
	glm::vec2 aimEnd = worldPos + aimDir * visualLength;

	ImVec2 screenStart = WorldToScreen(worldPos);
	ImVec2 screenEnd = WorldToScreen(aimEnd);

	constexpr ImU32 aimColor = IM_COL32(255, 0, 255, 255);
	drawList->AddLine(screenStart, screenEnd, aimColor, 2.0f);
	DrawArrow(drawList, screenStart, screenEnd, aimColor);
}

void DebugDrawSystem::DrawCameraWindow(const Entity *entity, ImDrawList *drawList) 	{
	auto *cameraSystem = ICameraSystem::Get();
	if(!cameraSystem) return;

	auto [camPos, windowSize] = cameraSystem->GetCameraWindowPosSize();
	glm::vec2 topLeft = glm::vec2(camPos.x - windowSize.x * 0.5f, camPos.y + windowSize.y * 0.5f);
	glm::vec2 bottomRight = glm::vec2(camPos.x + windowSize.x * 0.5f, camPos.y - windowSize.y * 0.5f);

	ImVec2 screenTopLeft = WorldToScreen(topLeft);
	ImVec2 screenBottomRight = WorldToScreen(bottomRight);

	ImU32 windowColor = IM_COL32(255, 255, 0, 255); // Yellow
	float lineThickness = 2.0f;

	drawList->AddRect(screenTopLeft, screenBottomRight, windowColor, 0.0f, 0, lineThickness);
}

void DebugDrawSystem::DrawColliderBox(const Entity *entity, ImDrawList *drawList) {
	auto *collider = entity->Get<Components::Collider>();
	if(!collider) return;

	auto *transform = entity->GetTransform();
	if(!transform) return;

	// Get world position and rotation
	glm::vec3 worldPos3D = transform->GetPosition();
	glm::vec2 worldPos(worldPos3D.x, worldPos3D.y);

	float worldRotation = transform->GetRotationRad();

	// Use actual collider size and offset instead of transform scale
	glm::vec3 colliderSize3D = collider->GetSize();
	glm::vec2 colliderSize(colliderSize3D.x, colliderSize3D.y);

	glm::vec3 colliderOffset3D = collider->GetOffset();
	glm::vec2 colliderOffset(colliderOffset3D.x, colliderOffset3D.y);

	// Calculate half extents from collider size
	glm::vec2 halfExtents = colliderSize * 0.5f;

	// Rotation matrix (2D)
	float cosAngle = std::cos(worldRotation);
	float sinAngle = std::sin(worldRotation);

	// Rotate the collider offset along with the entity, then compute the real center
	glm::vec2 rotatedOffset;
	rotatedOffset.x = colliderOffset.x * cosAngle - colliderOffset.y * sinAngle;
	rotatedOffset.y = colliderOffset.x * sinAngle + colliderOffset.y * cosAngle;
	glm::vec2 colliderCenter = worldPos + rotatedOffset;

	// Calculate the 4 corners of the rotated box in local space
	glm::vec2 localCorners[4] = {
		{-halfExtents.x, -halfExtents.y}, // Bottom-left
		{ halfExtents.x, -halfExtents.y}, // Bottom-right
		{ halfExtents.x,  halfExtents.y}, // Top-right
		{-halfExtents.x,  halfExtents.y}  // Top-left
	};

	// Transform corners to world space and convert to screen space
	ImVec2 screenCorners[4];
	for(int i = 0; i < 4; ++i) {
		// Apply rotation
		glm::vec2 rotated;
		rotated.x = localCorners[i].x * cosAngle - localCorners[i].y * sinAngle;
		rotated.y = localCorners[i].x * sinAngle + localCorners[i].y * cosAngle;

		// Apply translation from collider center (entity pos + rotated offset)
		glm::vec2 worldCorner = colliderCenter + rotated;

		// Convert to screen space
		screenCorners[i] = WorldToScreen(worldCorner);
	}

	// Draw the 4 edges of the rotated box
	ImU32 colliderColor = IM_COL32(0, 255, 0, 255); // Green
	float lineThickness = 2.0f;

	drawList->AddLine(screenCorners[0], screenCorners[1], colliderColor, lineThickness);
	drawList->AddLine(screenCorners[1], screenCorners[2], colliderColor, lineThickness);
	drawList->AddLine(screenCorners[2], screenCorners[3], colliderColor, lineThickness);
	drawList->AddLine(screenCorners[3], screenCorners[0], colliderColor, lineThickness);
}

void DebugDrawSystem::DrawTransformGizmo(const Entity *entity, ImDrawList *drawList) {
	auto *transform = entity->GetTransform();
	if(!transform) return;

	// Get world position and rotation
	glm::vec3 worldPos3D = transform->GetPosition();
	glm::vec2 worldPos(worldPos3D.x, worldPos3D.y);
	float worldRotation = transform->GetRotationRad();

	ImVec2 center = WorldToScreen(worldPos);

	// Gizmo size in world units (will be converted to screen space)
	float gizmoWorldSize = 0.5f;

	// Calculate rotated axis directions
	float cosAngle = std::cos(worldRotation);
	float sinAngle = std::sin(worldRotation);

	// X axis (local right) - rotated
	glm::vec2 xAxisWorld(cosAngle, sinAngle);
	glm::vec2 xAxisEnd = worldPos + xAxisWorld * gizmoWorldSize;
	ImVec2 xEnd = WorldToScreen(xAxisEnd);

	// Y axis (local up) - rotated
	glm::vec2 yAxisWorld(-sinAngle, cosAngle);
	glm::vec2 yAxisEnd = worldPos + yAxisWorld * gizmoWorldSize;
	ImVec2 yEnd = WorldToScreen(yAxisEnd);

	// Draw X axis (red)
	drawList->AddLine(center, xEnd, IM_COL32(255, 0, 0, 255), 3.0f);
	DrawArrow(drawList, center, xEnd, IM_COL32(255, 0, 0, 255));

	// Draw Y axis (green)
	drawList->AddLine(center, yEnd, IM_COL32(0, 255, 0, 255), 3.0f);
	DrawArrow(drawList, center, yEnd, IM_COL32(0, 255, 0, 255));
}

void DebugDrawSystem::DrawSpeedVector(const Entity *entity, ImDrawList *drawList) {
	auto *physics = entity->Get<Components::PhysicsBody>();
	auto *transform = entity->GetTransform();
	if(!physics || !transform) return;

	// Get world position (convert vec3 to vec2)
	glm::vec3 worldPos3D = transform->GetPosition();
	glm::vec2 worldPos(worldPos3D.x, worldPos3D.y);

	glm::vec3 velocity3d = physics->GetVelocity();
	glm::vec2 velocity = glm::vec2(velocity3d.x, velocity3d.y);
	// Don't draw if velocity is too small
	if(glm::length(velocity) < 0.01f) return;

	ImVec2 start = WorldToScreen(worldPos);

	// Scale velocity for better visualization
	glm::vec2 velocityEnd = worldPos + velocity * 0.5f;
	ImVec2 end = WorldToScreen(velocityEnd);

	// Draw yellow arrow for velocity
	drawList->AddLine(start, end, IM_COL32(255, 255, 0, 255), 2.0f);
	DrawArrow(drawList, start, end, IM_COL32(255, 255, 0, 255));
}

void DebugDrawSystem::DrawEntityName(const Entity *entity, ImDrawList *drawList) {
	auto *transform = entity->GetTransform();
	if(!transform) return;

	// Get world position (convert vec3 to vec2)
	glm::vec3 worldPos3D = transform->GetPosition();
	glm::vec2 worldPos(worldPos3D.x, worldPos3D.y);

	ImVec2 screenPos = WorldToScreen(worldPos);
	screenPos.y -= 25.0f; // Offset above entity

	const std::string_view &name = entity->Name(); 
	if(!name.empty()) {
		// Draw text with shadow for better visibility
		drawList->AddText(ImVec2(screenPos.x + 1, screenPos.y + 1),
			IM_COL32(0, 0, 0, 200), name.data());
		drawList->AddText(screenPos, IM_COL32(255, 255, 255, 255), name.data());
	}
}

ImVec2 DebugDrawSystem::WorldToScreen(const glm::vec2 &worldPos) const {
	auto *renderSystem = IRenderSystem::Get();
	auto *cameraSystem = ICameraSystem::Get();

	if(!renderSystem) {
		// Fallback
		return ImVec2(400.0f, 300.0f);
	}

	// === Step 1: Get transformation matrices ===
	float screenWidth = static_cast<float>(renderSystem->getScreenWidth());
	float screenHeight = static_cast<float>(renderSystem->getScreenHeight());
	float aspectRatio = screenWidth / screenHeight;

	// Projection matrix (same as RenderSystem)
	glm::mat4 projection = glm::ortho(-aspectRatio, aspectRatio, -1.0f, 1.0f, -1.0f, 1.0f);

	// View matrix (camera transform)
	glm::mat4 view = cameraSystem ? cameraSystem->GetViewMatrix() : glm::mat4(1.0f);

	// === Step 2: Transform world position through MVP pipeline ===
	// Convert 2D world position to 4D homogeneous coordinates
	glm::vec4 worldPos4D(worldPos.x, worldPos.y, 0.0f, 1.0f);

	// Apply view transformation (world space -> view space)
	glm::vec4 viewSpace = view * worldPos4D;

	// Apply projection transformation (view space -> clip space)
	glm::vec4 clipSpace = projection * viewSpace;

	// Perspective divide to get NDC (normalized device coordinates)
	// For orthographic projection, w = 1.0, but it's good practice
	glm::vec3 ndc = glm::vec3(clipSpace) / clipSpace.w;

	// === Step 3: Convert NDC [-1, 1] to screen space [0, width/height] ===
	float screenX = (ndc.x + 1.0f) * 0.5f * screenWidth;
	float screenY = (1.0f - ndc.y) * 0.5f * screenHeight;  // Y axis inverted

	return ImVec2(screenX, screenY);
}

void DebugDrawSystem::DrawArrow(ImDrawList *drawList, const ImVec2 &start, const ImVec2 &end, ImU32 color) {
	// Calculate direction vector
	ImVec2 dir = ImVec2(end.x - start.x, end.y - start.y);
	float length = std::sqrt(dir.x * dir.x + dir.y * dir.y);
	if(length < 1e-6f) return;

	dir.x /= length;
	dir.y /= length;

	// Calculate perpendicular vector
	ImVec2 perp = ImVec2(-dir.y, dir.x);

	// Draw arrowhead
	float headSize = 10.0f;
	ImVec2 arrowBase = ImVec2(end.x - dir.x * headSize, end.y - dir.y * headSize);

	drawList->AddTriangleFilled(
		end,
		ImVec2(arrowBase.x + perp.x * headSize * 0.5f, arrowBase.y + perp.y * headSize * 0.5f),
		ImVec2(arrowBase.x - perp.x * headSize * 0.5f, arrowBase.y - perp.y * headSize * 0.5f),
		color
	);
}
void DebugDrawSystem::DrawHealthBar(const Entity *entity, ImDrawList *drawList) {
	auto *health = entity->Get<Components::HealthComponent>();
	if(!health) return;
	if(health->isPlayer) return;
	auto *transform = entity->GetTransform();
	if(!transform) return;

	glm::vec3 worldPos3D = transform->GetPosition();
	ImVec2 screenPos = WorldToScreen(glm::vec2(worldPos3D.x, worldPos3D.y));

	constexpr float barWidth = 50.0f;
	constexpr float barHeight = 6.0f;
	constexpr float yOffset = 60.0f;

	float ratio = static_cast<float>(health->GetHealth()) /
		static_cast<float>(health->GetMaxHealth());

	ImVec2 bgMin{screenPos.x - barWidth * 0.5f, screenPos.y - yOffset};
	ImVec2 bgMax{screenPos.x + barWidth * 0.5f, screenPos.y - yOffset + barHeight};
	ImVec2 fgMax{bgMin.x + barWidth * ratio,    bgMax.y};

	drawList->AddRectFilled(bgMin, bgMax, IM_COL32(80, 0, 0, 200));
	drawList->AddRectFilled(bgMin, fgMax, IM_COL32(220, 40, 40, 220));
	drawList->AddRect(bgMin, bgMax, IM_COL32(255, 255, 255, 180));
}
} // namespace RassEngine::Systems
