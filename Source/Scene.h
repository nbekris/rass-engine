// File Name:    Scene.h
// Author(s):    main Taro Omiya, secondary Niko Bekris, Steven Yacoub, Boyuan Hu
// Course:       GAM541
// Project:      RASS
// Purpose:      Base scene class.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <memory>
#include <string>
#include <string_view>

#include "Entity.h"
#include "EntityContainer.h"
#include "Events/GlobalEventArgs.h"
#include "Events/GlobalEventListener.h"
#include "IEvent.h"
#include "Object.h"
#include "UUID.h"

namespace RassEngine {

// Taro's notes: choosing to make scenes non-cloneable for now
class Scene : public Object {
public:
	Scene(const std::string_view &name);
	virtual ~Scene() override = 0;

	inline const std::string &Name() const {
		return name;
	}

	// Inherited via Object
	virtual bool Initialize() override;

	virtual bool Shutdown();
	/// <summary>
	/// Adds an entity to this scene
	/// </summary>
	inline void AddEntity(std::unique_ptr<Entity> &&e) {
		entities.Add(std::move(e));
	}

	/// <summary>
	/// Removes an entity from this scene
	/// </summary>
	inline void RemoveEntity(const Entity *e) {
		entities.Remove(e);
	}

	/// <summary>
	/// Finds an entity with the specified UUID from this scene
	/// </summary>
	inline Entity* FindEntity(const UUID &id) const {
		return entities.Find(id);
	}

	/// <summary>
	/// Find an entity with the specified name from this scene.
	/// If there are multiple entities with the same name, a random one will be returned.
	/// </summary>
	inline Entity *FindEntity(const std::string_view &name) const {
		return entities.Find(name);
	}

	/// <summary>
	/// Finds a list of entities with the specified name from this scene.
	/// </summary>
	inline const std::vector<Entity *> *FindEntities(const std::string_view &name) const {
		return entities.FindAll(name);
	}

	/// <summary>
	/// Indicates whether the scene is ready.
	/// </summary>
	inline bool IsReady() const {
		return isInitialized;
	}

	// === NEW: Provide read-only access to entities ===

	/// <summary>
	/// Get all entities in this scene.
	/// Returns const reference to avoid copying.
	/// </summary>
	inline const EntityContainer &GetEntities() const {
		return entities;
	}

private:
	// Helper functions
	bool OnAfterUpdate(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &);

protected:
	std::string name;

private:
	EntityContainer entities;
	Events::GlobalEventListener<Scene> afterUpdateListener;
	bool isInitialized{false};
};

}
