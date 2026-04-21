// File Name:    Engine.h
// Author(s):    main Taro Omiya, secondary Niko Bekris, Boyuan Hu
// Course:       GAM541
// Project:      RASS
// Purpose:      Core engine class coordinating all game systems.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <mutex>
#include <memory>
#include <GLFW/glfw3.h>

#include "Systems/Manager/SystemsManager.h"

// Forward declaration
namespace RassEngine::Events {
class GlobalEventID;
}

namespace RassEngine {

// Forward declarations
namespace Systems {
class IGlobalEventsSystem;
class IRenderSystem;
}

/// <summary>
/// Singleton implementation of the engine
/// </summary>
class Engine {
public:
	/// <summary>
	/// Constructs a new engine
	/// </summary>
	Engine(std::unique_ptr<Systems::SystemsManager> &&systems);
	// Obligatory destructor
	virtual ~Engine();

	/// <summary>
	/// Indicates whether this engine is setup or not
	/// </summary>
	/// <returns>
	/// True if all the engine member variables are setup correctly.
	/// </returns>
	bool canRun() const;

	/// <summary>
	/// Runs the game loop.
	/// </summary>
	void run();

	/// <summary>
	/// Creates a window using GLFW. This is a helper function for the main function, and is not intended to be used by any other class.
	/// </summary>
	/// <remarks>
	/// (temporary refactor for now)
	/// </remarks>
	/// <returns>
	/// Pointer to the created GLFW window, or <c>nullptr</c> if anything failed
	/// </returns>
	static std::shared_ptr<GLFWwindow> CreateGlfwWindow();

private:
	// Prevent the default constructor from being accessible
	Engine() = delete;

	// Delete the rest of the operations
	Engine(const Engine &) = delete;
	Engine(Engine &&) noexcept = delete;
	Engine &operator=(const Engine &) = delete;
	Engine &operator=(Engine &&) noexcept = delete;

	bool CallEvent(const Events::GlobalEventID &id) const;

	//////////////////////////////////////////////////
	// Member Variables
	//////////////////////////////////////////////////
	/// <summary>
	/// Container of all systems.
	/// </summary>
	std::unique_ptr<Systems::SystemsManager> systemsManager = nullptr;
};

}
