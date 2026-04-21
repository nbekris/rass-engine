// File Name:    Entity.h
// Author(s):    main Taro Omiya, secondary Niko Bekris, Boyuan Hu, Steven Yacoub
// Course:       GAM541
// Project:      RASS
// Purpose:      Game entity base class.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <concepts>
#include <memory>
#include <string_view>
#include <string>
#include <vector>

#include "Component.h"
#include "Events/EntityEventID.h"
#include "Events/EventArgs.h"
#include "Events/EventsManager.h"
#include "ICloneable.h"
#include "ISerializable.h"
#include "Object.h"
#include "Stream.h"
#include "Utils.h"

// forward declaration
namespace RassEngine::Components {
class Transform;
}
namespace RassEngine::Systems {
class EntityFactory;
}

namespace RassEngine {

/// <summary>
/// Collection of components
/// </summary>
class Entity : public Object, public ICloneable<Entity>, public ISerializable<Stream> {
	friend class RassEngine::Systems::EntityFactory;
public:
	static constexpr std::string_view KEY_ENTITY = "Entity";
public:
	Entity();
	Entity(const Entity &other);
	virtual ~Entity(void);

public:
	// Inherited via Object
	virtual bool Initialize() override;
	virtual const std::string_view &NameClass() const override;

	// Inherited via ICloneable
	std::unique_ptr<Entity> Clone() const override;

	// Inherited via ISerializable
	bool Read(Stream &stream) override;

	/// <summary>
	/// Adds an existing component to this entity.
	/// Note: the pointer's ownership will be transferred to this entity.
	/// </summary>
	/// <param name="component">The component to add</param>
	void AddComponent(std::unique_ptr<Component> &&component);

	/// <summary>
	/// Constructs a new component, and returns
	/// the borrowed pointer.
	/// </summary>
	/// <typeparam name="T">Component to create</typeparam>
	/// <returns></returns>
	template<class T, typename... A> requires IsDerived<T, Component>
	T *MakeComponent(A&&... args);

	inline void Name(const std::string_view &newName) {
		name = newName;
	}

	void Destroy();

	inline bool IsDestroyed() const {
		return isDestroyed;
	}

	inline bool IsNamed(const std::string_view &compare) const {
		return name == compare;
	}

	/// <summary>
	/// Retrieves the first component based on type.
	/// Note: due to constant <c>dynamic_cast</c>-ing,
	/// this function is *slow*.  It is strongly recommended
	/// to cache its results.
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <returns></returns>
	template<class T> requires std::derived_from<T, Component>
	T *Get() const;

	/// <summary>
	/// Retrieves all the components attached to this entity,
	/// based on type.
	/// Note: due to constant <c>dynamic_cast</c>-ing,
	/// this function is *slow*.  It is strongly recommended
	/// to cache its results.
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <returns></returns>
	template<class T> requires std::derived_from<T, Component>
	std::vector<T *> GetAll() const;

	/// <summary>
	/// Helper function to retrieve the Transform component.
	/// Unlike <code>::Get()</code>, this method does cache.
	/// </summary>
	Components::Transform *GetTransform() const;

	const std::string_view Name() const {
		return name;
	}

	inline void BindEvent(const Events::EntityEventID &eventID, IEventListener<Events::EventArgs> *listener, Events::CallFrequency frequency = Events::CallFrequency::EveryCall) {
		events.bind(eventID, listener, frequency);
	}

	inline void UnbindEvent(const Events::EntityEventID &eventID, IEventListener<Events::EventArgs> *listener) {
		events.unbind(eventID, listener);
	}

	inline bool DispatchEntityEvent(const Events::EntityEventID &eventID, const Events::EventArgs &args) {
		return events.call(eventID, args);
	}

private:
	// Helper methods
	void ReadNewComponent(Stream &stream, const std::string &key);
	void ReadOverrideComponent(Stream &stream, const std::string &key);
	/// <summary>
	/// This is a helper method to set the ID of this entity, since the ID is const.
	/// It should only be used by EntityFactory, and should not be exposed publicly.
	/// </summary>
	void SetID(const UUID &newID);

private:
	// Member variables
	std::string name{};
	std::vector<std::unique_ptr<Component>> components{};
	std::unordered_map<std::string, std::vector<Component*>> nameToComponentCache{};
	bool isDestroyed{false};
	Events::EventsManager<Events::EntityEventID, Events::EventArgs> events;
	mutable Components::Transform *cacheTransform{nullptr};
};

template<class T, typename ...A> requires IsDerived<T, Component>
inline T *Entity::MakeComponent(A && ...args) {
	// First, create and add this component into the vector
	std::unique_ptr<T> toAdd = std::make_unique<T>(args...);
	T *toReturn = toAdd.get();
	AddComponent(std::move(toAdd));
	return toReturn;
}

template<class T> requires std::derived_from<T, Component>
inline T *Entity::Get() const {
	// Search through all of the attached components.
	T *toReturn = nullptr;
	for(const auto &component : components) {

		// Check if the component can be casted
		if(toReturn = dynamic_cast<T *>(component.get())) {
			// Return the matching component.
			break;
		}
	}
	return toReturn;
}

template<class T> requires std::derived_from<T, Component>
inline std::vector<T *> Entity::GetAll() const {
	// Search through all of the attached components.
	std::vector<T *> toReturn{components.size()};
	for(std::unique_ptr<Component> &component : components) {

		// Check if the component can be casted
		if(T *toAdd = dynamic_cast<T *>(component.get())) {
			// Add the instance to the return list
			toReturn.emplace(toAdd);
		}
	}

	// Shrink the vector to the actual number of components
	toReturn.shrink_to_fit();
	return toReturn;
}

}
