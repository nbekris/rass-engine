// File Name:    SystemsManagerBuilder.cpp
// Author(s):    main Taro Omiya, secondary Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Builder for configuring and constructing the SystemsManager.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "SystemsManagerBuilder.h"

#include <memory>
#include <utility>

#include "SystemsManager.h"
#include "Systems/Logging/ILoggingSystem.h"

namespace RassEngine::Systems {

SystemsManagerBuilder::SystemsManagerBuilder()
	: toBuild(std::unique_ptr<SystemsManager>(new SystemsManager())) {
}

std::unique_ptr<SystemsManager> SystemsManagerBuilder::build() {
	// Return the systems manager
	if(toBuild->Initialize()) {
		return std::move(toBuild);
	}

	// Otherwise, log an error
	if(ILoggingSystem::Get() != nullptr) {
		LOG_ERROR("At least one system registered in {} failed to initialize.", NAMEOF(Systems::SystemsManager));
		return nullptr;
	}

	// The worst-case scenario, print the error to console directly
	std::println(stderr, "A system registered in {} failed to initialize.", NAMEOF(Systems::SystemsManager));
	return nullptr;
}

}
