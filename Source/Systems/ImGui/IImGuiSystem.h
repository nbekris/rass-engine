// File Name:    IImGuiSystem.h
// Author(s):    main Boyuan Hu, secondary Taro Omiya, Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      System providing ImGui debug UI rendering.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include "Systems/GlobalEvents/IGlobalSystem.h"

struct GLFWwindow;

namespace RassEngine::Systems {

/// <summary>
/// Interface for ImGui management system.
/// Handles ImGui initialization, frame lifecycle, and rendering.
/// </summary>
class IImGuiSystem : public IGlobalSystem<IImGuiSystem> {
public:
	virtual ~IImGuiSystem() = default;

	/// <summary>
	/// Begin a new ImGui frame. Called at the start of each render frame.
	/// </summary>
	virtual void BeginFrame() = 0;

	/// <summary>
	/// End the ImGui frame and render all draw data. Called at the end of each render frame.
	/// </summary>
	virtual void EndFrame() = 0;

	/// <summary>
	/// Check if ImGui is initialized and ready to use.
	/// </summary>
	virtual bool IsReady() const = 0;

	/// <summary>
	/// If set to true (default,) allows ImGui to override
	/// mouse cursor behavior.
	/// </summary>
	/// <returns>True if operation succeeded</returns>
	virtual bool AllowCursorOverride(bool override = true) = 0;
};

} // namespace RassEngine::Systems
