// File Name:    IInputSystem.h
// Author(s):    main Taro Omiya, secondary Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      System processing keyboard, mouse, and gamepad input via GLFW.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <cstddef>
#include <glm/vec2.hpp>

#include "../GlobalEvents/IGlobalSystem.h"

namespace RassEngine::Systems {

class IInputSystem : public IGlobalSystem<IInputSystem> {
public:
	virtual ~IInputSystem() = default;

	/// <summary>
	/// Checks if a specific key is down.
	/// </summary>
	/// <param name="key"></param>
	/// <returns></returns>
	virtual bool IsKeyDown(unsigned short key) const = 0;
	/// <summary>
	/// Checks if a specific key is pressed *this frame*.
	/// </summary>
	/// <param name="key"></param>
	/// <returns></returns>
	virtual bool IsKeyPressed(unsigned short key) const = 0;
	/// <summary>
	/// Checks if a specific key is released *this frame*.
	/// </summary>
	/// <param name="key"></param>
	/// <returns></returns>
	virtual bool IsKeyReleased(unsigned short key) const = 0;
	/// <summary>
	/// Checks if a specific mouse button is down.
	/// </summary>
	/// <param name="key">GLFW_MOUSE_BUTTON_LEFT, GLFW_MOUSE_BUTTON_RIGHT, or GLFW_MOUSE_BUTTON_MIDDLE</param>
	/// <returns></returns>
	virtual bool IsMouseButtonDown(unsigned short button) const = 0;
	/// <summary>
	/// Checks if a specific key is clicked *this frame*.
	/// </summary>
	/// <param name="key">GLFW_MOUSE_BUTTON_LEFT, GLFW_MOUSE_BUTTON_RIGHT, or GLFW_MOUSE_BUTTON_MIDDLE</param>
	/// <returns></returns>
	virtual bool IsMouseButtonClicked(unsigned short button) const = 0;
	/// <summary>
	/// Checks if a specific key is released *this frame*.
	/// </summary>
	/// <param name="key">GLFW_MOUSE_BUTTON_LEFT, GLFW_MOUSE_BUTTON_RIGHT, or GLFW_MOUSE_BUTTON_MIDDLE</param>
	/// <returns></returns>
	virtual bool IsMouseButtonReleased(unsigned short button) const = 0;
	/// <summary>
	/// Gets the mouse position relative to the viewport.
	/// Coordinates will be set from -1 to 1, with (0, 0) at the center of the viewport,
	/// and (1, 1) set to the upper-right hand corner of the viewport
	/// </summary>
	/// <returns>
	/// Mouse position, relative to viewport dimensions.
	/// </returns>
	virtual const glm::vec2 &GetMousePositionViewport() const = 0;
	/// <summary>
	/// Retrieves the raw mouse position.
	/// </summary>
	/// <returns>
	/// Mouse position, as reported by GLFW.
	/// </returns>
	/// <remarks>
	/// Intended for debugging purposes.
	/// </remarks>
	virtual const glm::dvec2 &GetMousePositionRaw() const = 0;
	/// <summary>
	/// Sets whether the cursor is visible.  If false, the cursor will be hidden and captured by the game window.
	/// </summary>
	virtual void SetCursorVisible(bool visible) = 0;

protected:
	// Do not allow constructing an interface
	IInputSystem() = default;

private:
	// Remove the rest of the default functions
	IInputSystem(const IInputSystem &) = delete;
	IInputSystem(IInputSystem &&) noexcept = delete;
	IInputSystem &operator=(const IInputSystem &) = delete;
	IInputSystem &operator=(IInputSystem &&) noexcept = delete;
};

}
