// File Name:    ILoggingSystem.h
// Author(s):    main Taro Omiya, secondary Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      System for logging debug and error messages.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include "Precompiled.h"

#include <cstdio>
#include <intrin.h>
#include <print>
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>

#include "Systems/GlobalEvents/IGlobalSystem.h"

// Forward declarations
namespace RassEngine::Systems {
class SystemsManager;
class ILoggingSystem;
}

// Macros to easily use the Logger marked by the different levels we need
#define LOG_ERROR(...) \
if (const auto* system = RassEngine::Systems::ILoggingSystem::Get()) {\
	SPDLOG_LOGGER_ERROR(system->Logger(), __VA_ARGS__); \
} else { \
	std::println(stderr, "Error: {} doesn't have a {} registered.", NAMEOF(Systems::SystemsManager), NAMEOF(Systems::ILoggingSystem)); \
}

#define LOG_WARNING(...) \
if (const auto* system = RassEngine::Systems::ILoggingSystem::Get()) {\
	SPDLOG_LOGGER_WARN(system->Logger(), __VA_ARGS__); \
} else { \
	std::println(stderr, "Error: {} doesn't have a {} registered.", NAMEOF(Systems::SystemsManager), NAMEOF(Systems::ILoggingSystem)); \
}

#define LOG_DEBUG(...) \
if (const auto* system = RassEngine::Systems::ILoggingSystem::Get()) {\
	SPDLOG_LOGGER_DEBUG(system->Logger(), __VA_ARGS__); \
} else { \
	std::println(stderr, "Error: {} doesn't have a {} registered.", NAMEOF(Systems::SystemsManager), NAMEOF(Systems::ILoggingSystem)); \
}

#define LOG_INFO(...) \
if (const auto* system = RassEngine::Systems::ILoggingSystem::Get()) {\
	SPDLOG_LOGGER_INFO(system->Logger(), __VA_ARGS__); \
} else { \
	std::println(stderr, "Error: {} doesn't have a {} registered.", NAMEOF(Systems::SystemsManager), NAMEOF(Systems::ILoggingSystem)); \
}

#define LOG_TRACE(...) \
if (const auto* system = RassEngine::Systems::ILoggingSystem::Get()) {\
	SPDLOG_LOGGER_TRACE(system->Logger(), __VA_ARGS__); \
} else { \
	std::println(stderr, "Error: {} doesn't have a {} registered.", NAMEOF(Systems::SystemsManager), NAMEOF(Systems::ILoggingSystem)); \
}

#define LOG_ASSERT(x, ...) { if(!(x)) { LOG_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }

namespace RassEngine::Systems {

class ILoggingSystem : public IGlobalSystem<ILoggingSystem> {
public:
	virtual ~ILoggingSystem() = default;
	virtual spdlog::logger *Logger() const = 0;
};

}
