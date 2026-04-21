// File Name:    IDebugDrawSystem.h
// Author(s):    main Boyuan Hu, secondary Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      System rendering debug shapes and overlays.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <glm/glm.hpp>
#include <imgui.h>
#include "Systems/GlobalEvents/IGlobalSystem.h"

namespace RassEngine {
class Entity;
}

namespace RassEngine::Systems {

/// <summary>
/// Interface for the debug visualization system.
/// Provides debug drawing capabilities for development purposes.
/// </summary>
class IDebugDrawSystem : public IGlobalSystem<IDebugDrawSystem> {
public:
    virtual ~IDebugDrawSystem() = default;

    // === Global debug flags ===
    virtual bool IsDebugEnabled() const = 0;
    virtual void SetDebugEnabled(bool enabled) = 0;
    
    virtual bool ShouldShowColliderBoxes() const = 0;
    virtual bool ShouldShowTransformGizmos() const = 0;
    virtual bool ShouldShowEntityNames() const = 0;
    virtual bool ShouldShowSpeedVectors() const = 0;
    virtual bool ShouldShowMeshWireframe() const = 0;
    virtual bool ShouldShowFPS() const = 0;
		virtual bool ShouldShowAimDirections() const = 0;

    // === Main rendering function (called by RenderSystem) ===
    virtual void RenderDebugVisualization() = 0;

    // === Utility functions for coordinate conversion ===
    virtual ImVec2 WorldToScreen(const glm::vec2& worldPos) const = 0;
    virtual void DrawArrow(ImDrawList* drawList, const ImVec2& start, const ImVec2& end, ImU32 color) = 0;
};

} // namespace RassEngine::Systems
