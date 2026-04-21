// File Name:    CursorMode.cpp
// Author(s):    main Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Component controlling the mouse cursor display mode.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "Components/CursorMode.h"

#include <string_view>

#include "Cloneable.h"
#include "Component.h"
#include "Stream.h"
#include "Systems/Input/IInputSystem.h"
#include "Systems/Logging/ILoggingSystem.h"
#include "Utils.h"

namespace RassEngine::Components {
using namespace RassEngine::Systems;

CursorMode::CursorMode(): Cloneable<Component, CursorMode>{} {}

CursorMode::CursorMode(const CursorMode &other)
	: Cloneable<Component, CursorMode>{other}
	, isCursorVisible{other.isCursorVisible} {}

CursorMode::~CursorMode() {}

bool CursorMode::Initialize() {
	if(IInputSystem::Get() == nullptr) {
		LOG_ERROR("{}: Input system not found when initializing CursorMode component", NameClass());
		return false;
	}

	IInputSystem::Get()->SetCursorVisible(isCursorVisible);
	return true;
}

bool CursorMode::Read(Stream &stream) {
	if(!Component::Read(stream)) {
		return false;
	}

	stream.Read("Visible", isCursorVisible);
	return true;
}

const std::string_view &CursorMode::NameClass() const {
	static constexpr std::string_view className = NAMEOF(RassEngine::Components::CursorMode);
	return className;
}

}
