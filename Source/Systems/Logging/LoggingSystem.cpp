// File Name:    LoggingSystem.cpp
// Author(s):    main Taro Omiya, secondary Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      System for logging debug and error messages.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "LoggingSystem.h"

#include <iterator>
#include <spdlog/common.h>
#include <spdlog/logger.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/msvc_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog-inl.h>
#include <windows.h>
#include <shlobj.h>
#include <string>
#include <filesystem>

#include "Utils.h"

#ifdef _DEBUG
#pragma warning( push )
#pragma warning( disable : 4005 )
// Enable more info to show on logs under debug mode
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#pragma warning( pop )
#endif // _DEBUG

using namespace RassEngine::Systems;

static const std::string_view GAME_NAME = "GameRass";
static const std::string_view LOG_FOLDER_NAME = "Logs";
static const char* LOG_FILE_NAME = "Trace.log";

/// <summary>
/// Sets up the the logger sinks.
/// Doing it here, so the logger is available
/// for other Systems to use when calling the
/// logger in ISystem::Initialize()
/// </summary>
LoggingSystem::LoggingSystem() : _logger{nullptr} {
	// Sinks are where the logger will output to.
	std::vector<spdlog::sink_ptr> _sinks;

	// First sink is for the Console Logger
	_sinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
#ifdef _DEBUG
	_sinks[0]->set_pattern("%^%T [%l] [%s] %!(), line %#:\n  %v%$");
#else
	_sinks[0]->set_pattern("%^%T [%l] %n: %v%$");
#endif // _DEBUG

	// Second sink is for outputting an error log file
#ifdef _DEBUG
	_sinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(LOG_FILE_NAME, true));
#else
	std::wstring base_path = GetLocalAppdataPath();
	if(!base_path.empty()) {
		// Create a subfolder for your app: AppData/Local/GameRass/logs/
		std::filesystem::path log_dir = std::filesystem::path(base_path) / GAME_NAME / LOG_FOLDER_NAME;

		try {
			// Ensure the directory exists (spdlog won't always create the tree for you)
			if(!std::filesystem::exists(log_dir)) {
				std::filesystem::create_directories(log_dir);
			}

			std::filesystem::path log_file = log_dir / LOG_FILE_NAME;
			_sinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(log_file.string(), true));
		} catch(const spdlog::spdlog_ex &ex) {
			// Handle initialization failure
			_sinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(LOG_FILE_NAME, true));
		}
	}
#endif

#ifdef _DEBUG
	_sinks[1]->set_pattern("%T [%l] [%s] %!(), line %#:\n  %v");
#else
	_sinks[1]->set_pattern("%T [%l] %n: %v");
#endif // _DEBUG

	// Third sink is for Visual Studio's output tab
	_sinks.emplace_back(std::make_shared<spdlog::sinks::msvc_sink_mt>());
#ifdef _DEBUG
	_sinks[2]->set_pattern("%T [%l] [%s] %!(), line %#:\n  %v");
#else
	_sinks[2]->set_pattern("%T [%l] %n: %v");
#endif // _DEBUG

	// Initializing the logger
	_logger = std::make_shared<spdlog::logger>("LOG", begin(_sinks), end(_sinks));

	spdlog::register_logger(_logger);

	_logger->set_level(spdlog::level::trace);
	_logger->flush_on(spdlog::level::trace);
}

LoggingSystem::~LoggingSystem() {
	_logger = nullptr;
}

spdlog::logger *LoggingSystem::Logger() const {
	return _logger.get();
}

bool LoggingSystem::Initialize() {
	return true; // Do nothing, constructor took care of everything
}

const std::string_view &LoggingSystem::NameClass() const {
	static constexpr std::string_view className = NAMEOF(RassEngine::Systems::LoggingSystem);
	return className;
}

void LoggingSystem::Shutdown() {}

std::wstring LoggingSystem::GetLocalAppdataPath() {
	PWSTR path_tmp = nullptr;
	// FOLDERID_LocalAppData is the standard for non-roaming application data
	HRESULT hr = SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &path_tmp);

	if(FAILED(hr)) {
		return std::wstring(); // Fallback or handle error
	}

	std::wstring ws(path_tmp);
	CoTaskMemFree(path_tmp); // Free the memory allocated by the API

	return ws;
}
