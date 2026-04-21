// File Name:    SystemsManager.cpp
// Author(s):    main Taro Omiya, secondary Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Manager initializing and updating all engine systems.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "SystemsManager.h"

#include <string_view>

#include "Systems/Logging/ILoggingSystem.h"

namespace RassEngine::Systems {

SystemsManager::SystemsManager() : typeToInstanceMap() {
}

bool SystemsManager::Initialize() {
	// Initialize all systems
	bool isSuccessful = true;
	for(auto &[type, pair] : typeToInstanceMap) {
		// Check if initialization succeeded
		bool result = pair.system->Initialize();
		if (result) {
			// Move to the next system
			continue;
		}

		// Indicate which system failed to initialize
		isSuccessful = false;

		// Log an error, if the logger is available
		if(ILoggingSystem::Get() != nullptr) {
			LOG_ERROR("System \"{}\" failed to initialize.", type.name());
			continue;
		}

		// The worst-case scenario, print the error to console directly
		std::println(stderr, "System \"{}\" failed to initialize.", type.name());
	}
	return isSuccessful;
}

void SystemsManager::Shutdown() {
	// Setup priority list
	std::vector<std::tuple<std::type_index, size_t>> typeAndPriorityPair;
	typeAndPriorityPair.reserve(typeToInstanceMap.size());

	// Call shutdown on all systems, first
	for(auto &[type, pair] : typeToInstanceMap) {
		pair.system->Shutdown();
		typeAndPriorityPair.emplace_back(type, pair.priority);
	}

	// Sort the systems by priority
	std::sort(typeAndPriorityPair.begin(), typeAndPriorityPair.end(),
		[] (std::tuple<std::type_index, size_t> &left, std::tuple<std::type_index, size_t> &right) {
			return std::get<1>(left) < std::get<1>(right);
		}
	);

	// Unregister all the systems, next
	for(auto &[_, pair] : typeToInstanceMap) {
		pair.unregisterSystem();
	}

	// Clear all member variables, in priority order
	for(const auto &tuple : typeAndPriorityPair) {
		typeToInstanceMap.erase(std::get<0>(tuple));
	}
	typeToInstanceMap.clear();
}

const std::string_view &SystemsManager::NameClass() const {
	static constexpr std::string_view className = NAMEOF(SystemsManager);
	return className;
}

SystemsManager::~SystemsManager() {
	Shutdown();
}

}
