// File Name:    SystemsManager.h
// Author(s):    main Taro Omiya, secondary Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Manager initializing and updating all engine systems.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <concepts>
#include <memory>
#include <stdexcept>
#include <string_view>
#include <typeindex>
#include <unordered_map>

#include "../GlobalEvents/GlobalEventsSystem.h"
#include "ISystem.h"

namespace RassEngine::Systems {

// Forward declaration
class SystemsManagerBuilder;

/// <summary>
/// Map of interface-to-concrete class instances
/// </summary>
class SystemsManager : ISystem {
	// Allow the builder class to accessthis class
	friend class SystemsManagerBuilder;

public:
	using PriorityType = int;

	virtual ~SystemsManager();

	// Inherited via ISystem
	const std::string_view &NameClass() const override;
	bool Initialize() override;
	void Shutdown() override;

	/// <summary>
	/// Retrieves a pointer to a shared <see cref="ISystem"/> instace, if initialized.
	/// </summary>
	/// <returns>
	/// Pointer to a shared <see cref="ISystem"/> instace, or <c>nullptr</c>.
	/// </returns>
	template<IsSystemInterface I>
	I* get() const;

private:
	// Make all constructors private
	SystemsManager();

	// Delete the rest of the operations
	SystemsManager(const SystemsManager &) = delete;
	SystemsManager(SystemsManager &&) noexcept = delete;
	SystemsManager &operator=(const SystemsManager &) = delete;
	SystemsManager &operator=(SystemsManager &&) noexcept = delete;

	// Make register and unregister only accessible to builder
	template<IsSystemInterface I>
	void Register(std::unique_ptr<I> &&instance);
	template<IsSystemInterface I>
	void Register(std::unique_ptr<I> &&instance, PriorityType priority);
	template<IsSystemInterface I>
	void Unregister();

	// Member variables
	struct SystemInstance {
		std::unique_ptr<ISystem> system;
		std::function<void()> unregisterSystem;
		PriorityType priority = 0;
	};
	std::unordered_map<std::type_index, SystemInstance> typeToInstanceMap;
};

template<IsSystemInterface I>
inline I* SystemsManager::get() const {
	// Attempt to find the instance of this class
	auto iterator = typeToInstanceMap.find(std::type_index(typeid(I)));
	if(iterator != typeToInstanceMap.end()) {
		// If found, return the value's borrowed pointer
		return dynamic_cast<I*>((*iterator).second.get());
	}

	// Otherwise, return null
	return nullptr;
}

template<IsSystemInterface I>
inline void SystemsManager::Register(std::unique_ptr<I> &&instance) {
	this->Register(std::move(instance), 0);
}

template<IsSystemInterface I>
inline void SystemsManager::Register(std::unique_ptr<I> &&instance, PriorityType priority) {
	if(instance == nullptr) {
		throw std::invalid_argument("Cannot register a null system instance.");
	}

	// Construct a new system pair
	SystemInstance pair;
	pair.system = std::move(instance);
	pair.priority = priority;
	pair.unregisterSystem = [] () {
		// Set the global instance variables, if any
		if(std::derived_from<I, IGlobalSystem<I>>) {
			IGlobalSystem<I>::Impl::Set(nullptr);
		}
	};

	// Insert the instance (override the last instance in the map, if necessary)
	const std::type_index index = std::type_index(typeid(I));
	typeToInstanceMap.insert_or_assign(index, std::move(pair));

	// Set the global instance variables, if any
	if(std::derived_from<I, IGlobalSystem<I>>) {
		I *registeredInstance = dynamic_cast<I *>(typeToInstanceMap.at(index).system.get());
		IGlobalSystem<I>::Impl::Set(registeredInstance);
	}
}

template<IsSystemInterface I>
inline void SystemsManager::Unregister() {
	// Find the entry in the map
	const std::type_index index = std::type_index(typeid(I));
	auto& it = typeToInstanceMap.find(index);
	if(it != typeToInstanceMap.end()) {
		// Unregister the system
		it->second.unregisterSystem();

		// Remove the entry from the map
		typeToInstanceMap.erase(index);
	}
}

}
