// File Name:    IFeelTrigger.h
// Purpose:      Engine-side contract so engine components can trigger
//               game-defined game-feel reactions without a hard dependency.
// Copyright © 2026 DigiPen (USA) Corporation.
#pragma once
#include <string_view>
#include <glm/vec3.hpp>

namespace RassEngine {
class IFeelTrigger {
public:
	virtual ~IFeelTrigger() = default;
	virtual bool Play(const std::string_view &eventName) = 0;
	virtual bool PlayDetached(const std::string_view &eventName) = 0;
	// For sources without an entity (e.g. a destroyed tile): caller supplies the world position.
	virtual bool PlayDetachedAt(const std::string_view &eventName, const glm::vec3 &worldPos) = 0;
};
}
