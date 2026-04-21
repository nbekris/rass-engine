// File Name:    LoggingSystem.h
// Author(s):    main Taro Omiya, secondary Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      System for logging debug and error messages.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <memory>
#include <spdlog/spdlog.h>
#include <string>

#include "ILoggingSystem.h"

namespace RassEngine::Systems {

class LoggingSystem : public ILoggingSystem {
public:
	LoggingSystem();
	~LoggingSystem() override;

	// Inherited via ILoggingSystem
	spdlog::logger *Logger() const override;
	bool Initialize() override;
	const std::string_view &NameClass() const override;
	void Shutdown() override;
private:
	static std::wstring GetLocalAppdataPath();

	std::shared_ptr<spdlog::logger> _logger;
};

}
