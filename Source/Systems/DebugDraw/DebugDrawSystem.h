// File Name:    DebugDrawSystem.h
// Author(s):    main Boyuan Hu, secondary Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      System rendering debug shapes and overlays.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <memory>
#include <string_view>
#include "IDebugDrawSystem.h"
#include "Events/GlobalEventListener.h"

namespace RassEngine {
class Scene;
class Entity;
}

namespace RassEngine::Systems {

/// <summary>
/// Concrete implementation of debug drawing system.
/// Handles all debug visualization including colliders, gizmos, and performance metrics.
/// </summary>
class DebugDrawSystem : public IDebugDrawSystem {
public:
    DebugDrawSystem();
    virtual ~DebugDrawSystem();

    // Inherited via ISystem
    bool Initialize() override;
    void Shutdown() override;
    const std::string_view& NameClass() const override;

    // Inherited via IDebugDrawSystem
    bool IsDebugEnabled() const override { return enableDebug_; }
    void SetDebugEnabled(bool enabled) override { enableDebug_ = enabled; }
    
    bool ShouldShowColliderBoxes() const override { return showColliderBoxes_; }
    bool ShouldShowTransformGizmos() const override { return showTransformGizmos_; }
    bool ShouldShowEntityNames() const override { return showEntityNames_; }
    bool ShouldShowSpeedVectors() const override { return showSpeedVectors_; }
    bool ShouldShowMeshWireframe() const override { return showMeshWireframe_; }
    bool ShouldShowFPS() const override { return showFPS_; }
		bool ShouldShowAimDirections() const override { return showAimDirections_; }
    void RenderDebugVisualization() override;
    ImVec2 WorldToScreen(const glm::vec2& worldPos) const override;
    void DrawArrow(ImDrawList* drawList, const ImVec2& start, const ImVec2& end, ImU32 color) override;

private:
    // === UI rendering ===
    void RenderDebugPanel();
    void RenderFPSOverlay();

    // === Entity traversal and drawing ===
    void DrawAllEntitiesDebugInfo(ImDrawList* drawList);
    void DrawEntityDebugInfo(const Entity* entity, ImDrawList* drawList);

    // === Specific drawing functions ===
    void DrawColliderBox(const Entity* entity, ImDrawList* drawList);
    void DrawTransformGizmo(const Entity* entity, ImDrawList* drawList);
    void DrawSpeedVector(const Entity* entity, ImDrawList* drawList);
    void DrawEntityName(const Entity* entity, ImDrawList* drawList);
		void DrawAimDirection(const Entity *entity, ImDrawList *drawList);
		void DrawCameraWindow(const Entity *entity, ImDrawList *drawList);

    // === Debug flags ===
    bool enableDebug_{true};
    bool showColliderBoxes_{false};
    bool showTransformGizmos_{false};
    bool showEntityNames_{false};
    bool showSpeedVectors_{false};
    bool showMeshWireframe_{false};
    bool showFPS_{true};
		bool showAimDirections_{false};
		bool showCameraWindow_{true};

    // === UI state ===
    bool showDebugPanel_{true};
    
    // === Performance tracking ===
    static constexpr int FPS_HISTORY_SIZE = 100;
    float fpsHistory_[FPS_HISTORY_SIZE]{};
    int fpsHistoryIndex_{0};

		
		bool showHealthBars_{true}; 
		void DrawHealthBar(const Entity *entity, ImDrawList *drawList);
};

} // namespace RassEngine::Systems
