// File Name:    EntityContainer.h
// Author(s):    main Niko Bekris, secondary Steven Yacoub, Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Container managing all active entities.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <string_view>
#include <unordered_map>

#include "UUID.h"

namespace RassEngine {
class Entity;

class EntityContainer {
	using Container = std::vector<std::unique_ptr<Entity>>;
public:
	EntityContainer();

	void Add(std::unique_ptr<Entity> &&e);
	void Remove(const Entity *e);
	bool CleanDestroyedEntities();
	Entity *Find(const UUID &id) const;
	Entity *Find(const std::string_view &name) const;
	const std::vector<Entity *> *FindAll(const std::string_view &name) const;

	inline bool isEmpty() const {
		return (container.size() == 0);
	}
private:
	void AddToCache(Entity *toAdd);
	void RemoveFromCache(const Entity *toRemove);
private:
	Container container;
	mutable std::unordered_map<UUID, Entity *> idToEntityCache;
	mutable std::unordered_map<std::string, std::vector<Entity *>> nameToEntityCache;

#pragma region Iterators
public:
	// Iterator code heavily inspired by:
	// https://jonasdevlieghere.com/post/containers-of-unique-pointers/
	/// <summary>
	/// Iterator wrapper for EntityContainer's list of entities.
	/// </summary>
	template <typename T>
	class Iterator : public T {
		friend class EntityContainer;
	public:
		// Necessary to make this container iterable
		using iterator_category = T::iterator_category;
		using difference_type = T::difference_type;
		using value_type = typename T::value_type::element_type;
		using pointer = value_type*;
		using reference = value_type&;

		inline reference operator*() const {
			return *(this->operator->());
		}
		inline pointer operator->() const {
			return this->T::operator*().get();
		}
		inline reference operator[](size_t n) const {
			return *(this->T::operator[](n));
		}

	private:
		inline Iterator(const T &&it) : T{it} {}
	};

public:
	using iterator = Iterator<Container::iterator>;
	using const_iterator = Iterator<Container::const_iterator>;

	inline iterator begin() noexcept {
		return iterator{std::begin(container)};
	}

	inline iterator end() noexcept {
		return iterator{std::end(container)};
	}

	inline const_iterator cbegin() const noexcept {
		return const_iterator{std::cbegin(container)};
	}

	inline const_iterator cend() const noexcept {
		return const_iterator{std::cend(container)};
	}
#pragma endregion
};

}
