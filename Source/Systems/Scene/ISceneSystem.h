// File Name:    ISceneSystem.h
// Author(s):    main Taro Omiya, secondary Boyuan Hu, Niko Bekris, Steven Yacoub
// Course:       GAM541
// Project:      RASS
// Purpose:      System managing scene loading and transitions.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <concepts>
#include <functional>
#include <memory>
#include <typeinfo>
#include <utility>

#include "../GlobalEvents/IGlobalSystem.h"

// Forward declaration
namespace RassEngine {
class Scene;
class Entity;
class UUID;
}

namespace RassEngine::Systems {

class ISceneSystem : public IGlobalSystem<ISceneSystem> {
public:
	// TODO: when a proper scene file has been defined, change this
	// function to allow any Scene object to be set.
	// Remove templates as well.
	// @brief Tell the scene system to switch to a new, pending scene.
	// @brief [NOTE: This function is templatized to avoid creating a new instance of a scene
	//   if it is already the active scene.]
	template <typename T> requires std::derived_from<T, Scene>
	inline void SetPendingScene() {
		SetPendingScene(typeid(T), [] () { return std::make_unique<T>(); });
	}

	// @brief Add a new entity to the active scene.
	virtual void AddEntity(std::unique_ptr<Entity> &&entity) = 0;

	// @brief Tell the scene system to end the active scene.
	// @brief [NOTE: The engine should shutdown when no active or pending scene(s) remain.]
	// @brief [NOTE: Any active scene is automatically deleted due to the nature of std::unique_ptr.]
	virtual void EndScene() = 0;

	// @brief Tell the scene system to restart the active scene.
	virtual void Restart() = 0;

	// @brief Determines if the system is still running.
	// @brief [NOTE: The engine should shutdown when no active or pending scene(s) remain.]
	//
	// @return bool = true if the scene system is still running, otherwise false.
	virtual bool IsRunning() const = 0;
	virtual bool IsSceneChanging() const = 0;
	virtual bool IsSceneRestarting() const = 0;


	/// <summary>
	/// Get the currently active scene.
	/// Returns nullptr if no scene is active.
	/// </summary>
	virtual Scene *GetCurrentScene() const = 0;

	/// <summary>
	/// Iterate through all entities in the current scene.
	/// Callback is invoked for each valid, non-destroyed entity.
	/// </summary>
	/// <param name="callback">Function to call for each entity</param>
	virtual void ForEachActiveEntity(std::function<void(const Entity *)> callback) const = 0;

	/// <summary>
	/// Finds the first active entity with the given name.
	/// </summary>
	/// <returns>Pointer to the entity, or nullptr if not found.</returns>
	virtual Entity *FindEntity(const std::string_view &name) const = 0;

	/// <summary>
	/// Finds the first active entity with the given UUID.
	/// </summary>
	/// <returns>Pointer to the entity, or nullptr if not found.</returns>
	virtual Entity *FindEntity(const UUID &id) const = 0;

	/// <summary>
	/// Finds the first active entity with the given UUID.
	/// </summary>
	/// <returns>Pointer to the entity, or nullptr if not found.</returns>
	virtual const std::vector<Entity*> *FindEntities(const std::string_view &name) const = 0;

	// === Template helper for scene transitions ===
	template <typename SceneType, class... _Types>
	static void SetPendingScene(_Types&&... args) {
		if(Get() == nullptr) {
			return;
		}
		Get()->SetPendingScene(typeid(SceneType), [...args = std::forward<_Types>(args)] { return std::make_unique<SceneType>(args...); });
	}

protected:
	// TODO: when a proper scene file has been defined, change this
	// function to be public, and allow any Scene object to be set.
	// Remove templates as well.
	virtual void SetPendingScene(const std::type_info &sceneType, std::function<std::unique_ptr<Scene>()> sceneGenerator) = 0;
};

}
