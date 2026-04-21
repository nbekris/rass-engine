// File Name:    InputSystemGlfw.h
// Author(s):    main Taro Omiya, secondary Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      System processing keyboard, mouse, and gamepad input via GLFW.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <cstddef>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include <string_view>

#include "Events/GlobalEventArgs.h"
#include "Events/GlobalEventListener.h"
#include "IInputSystem.h"
#include "IEvent.h"

namespace RassEngine::Systems {

class InputSystemGlfw : public IInputSystem {
public:
	static constexpr unsigned short KEY_QUIT = GLFW_KEY_ESCAPE;

	InputSystemGlfw(GLFWwindow *window);
	virtual ~InputSystemGlfw();

	/// <summary>
	/// Updates what keys are pressed
	/// </summary>
	/// <returns>True if update succeeds</returns>
	bool Update(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs&);

	// Inherited via Object
	const std::string_view &NameClass() const override;

	// Inherited via ISystem
	bool Initialize() override;
	void Shutdown() override;

	// Inherited via IInputSystem
	bool IsKeyDown(unsigned short key) const override;
	bool IsKeyPressed(unsigned short key) const override;
	bool IsKeyReleased(unsigned short key) const override;
	bool IsMouseButtonDown(unsigned short button) const override;
	bool IsMouseButtonClicked(unsigned short button) const override;
	bool IsMouseButtonReleased(unsigned short button) const override;

	inline const glm::dvec2 &GetMousePositionRaw() const override {
		return mousePosRaw;
	}
	inline const glm::vec2 &GetMousePositionViewport() const override {
		return mousePosViewport;
	}
	inline void SetCursorVisible(bool visible) override {
		glfwSetInputMode(window, GLFW_CURSOR, visible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
	}
private:
	// Remove the rest of the default functions
	InputSystemGlfw() = delete;
	InputSystemGlfw(const InputSystemGlfw &) = delete;
	InputSystemGlfw(InputSystemGlfw &&) noexcept = delete;
	InputSystemGlfw &operator=(const InputSystemGlfw &) = delete;
	InputSystemGlfw &operator=(InputSystemGlfw &&) noexcept = delete;

	// Helper functions
	void UpdateKeyChanges();
	void UpdateMousePositions();
	void OnMouseButtonChanged(int button, int action);
	void WhenMouseButtonDown(unsigned short button, bool &thisFrame, bool &lastFrame) const;

	// Member variables
	GLFWwindow *window;
	bool currentKeys[GLFW_KEY_LAST + 1];
	bool previousKeys[GLFW_KEY_LAST + 1];
	glm::dvec2 mousePosRaw;
	glm::vec2 mousePosViewport;
	std::byte currentMouseButtons{0};
	std::byte previousMouseButtons{0};
	Events::GlobalEventListener<InputSystemGlfw> updateListener;
#ifndef _DEBUG
	bool isMouseCaptured{false};
#endif
};

}
