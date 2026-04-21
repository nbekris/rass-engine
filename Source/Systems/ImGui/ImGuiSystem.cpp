// File Name:    ImGuiSystem.cpp
// Author(s):    main Boyuan Hu, secondary Taro Omiya, Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      System providing ImGui debug UI rendering.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "ImGuiSystem.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

#include "Systems/Logging/ILoggingSystem.h"
#include "Utils.h"

namespace RassEngine::Systems {

ImGuiSystem::ImGuiSystem(GLFWwindow *window)
	: window_{window} {
	if(window == nullptr) {
		throw std::invalid_argument("window cannot be null");
	}
}

ImGuiSystem::~ImGuiSystem() {
	Shutdown();
}

bool ImGuiSystem::Initialize() {
	if(!window_) {
		LOG_ERROR("ImGuiSystem: Cannot initialize without valid GLFW window");
		return false;
	}

	// Setup ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();

	// Optional: Enable docking and multi-viewport features
	// io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	// io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	// Setup ImGui backends
	if(!ImGui_ImplGlfw_InitForOpenGL(window_, true)) {
		LOG_ERROR("ImGuiSystem: Failed to initialize GLFW backend");
		return false;
	}

	if(!ImGui_ImplOpenGL3_Init("#version 330")) {
		LOG_ERROR("ImGuiSystem: Failed to initialize OpenGL3 backend");
		ImGui_ImplGlfw_Shutdown();
		return false;
	}

	// Setup ImGui style
	ImGui::StyleColorsDark();
	// Alternative styles:
	// ImGui::StyleColorsLight();
	// ImGui::StyleColorsClassic();

	isInitialized_ = true;
	LOG_INFO("ImGuiSystem initialized successfully");
	return true;
}

void ImGuiSystem::Shutdown() {
	if(!isInitialized_) {
		return;
	}

	// Shutdown ImGui backends
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();

	// Destroy ImGui context
	if(ImGui::GetCurrentContext() != nullptr) {
		ImGui::DestroyContext();
	}

	isInitialized_ = false;
	LOG_INFO("ImGuiSystem shutdown");
}

void ImGuiSystem::BeginFrame() {
	if(!isInitialized_) {
		return;
	}

	// Start new ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void ImGuiSystem::EndFrame() {
	if(!isInitialized_) {
		return;
	}

	// Render ImGui
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// Update and render additional platform windows (for multi-viewport)
	// ImGuiIO& io = ImGui::GetIO();
	// if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
	//     GLFWwindow* backup_current_context = glfwGetCurrentContext();
	//     ImGui::UpdatePlatformWindows();
	//     ImGui::RenderPlatformWindowsDefault();
	//     glfwMakeContextCurrent(backup_current_context);
	// }
}

bool ImGuiSystem::AllowCursorOverride(bool override) {
	if(!IsReady()) {
		return false;
	}

	// Check whether we want IMGUI to override mouse cursor behavior
	if (override) {
		// If so, enable mouse cursor change
		ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouseCursorChange;
	} else {
		// If not, disable it
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
	}
	return true;
}

const std::string_view &ImGuiSystem::NameClass() const {
	static constexpr std::string_view className = NAMEOF(Systems::ImGuiSystem);
	return className;
}

} // namespace RassEngine::Systems
