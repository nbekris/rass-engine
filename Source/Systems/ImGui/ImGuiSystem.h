// File Name:    ImGuiSystem.h
// Author(s):    main Boyuan Hu, secondary Taro Omiya, Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      System providing ImGui debug UI rendering.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <string_view>
#include "IImGuiSystem.h"

struct GLFWwindow;

namespace RassEngine::Systems {

/// <summary>
/// Concrete implementation of ImGui management system.
/// Handles ImGui context creation, backend initialization, and frame rendering.
/// </summary>
class ImGuiSystem : public IImGuiSystem {
public:
	explicit ImGuiSystem(GLFWwindow *window);
	virtual ~ImGuiSystem();

	// Inherited via ISystem
	bool Initialize() override;
	void Shutdown() override;
	const std::string_view &NameClass() const override;

	// Inherited via IImGuiSystem
	void BeginFrame() override;
	void EndFrame() override;
	inline bool IsReady() const override {
		return isInitialized_;
	}
	bool AllowCursorOverride(bool override) override;

private:
	GLFWwindow *window_;
	bool isInitialized_{false};
};

} // namespace RassEngine::Systems
