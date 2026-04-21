// File Name:    IEntityFactory.h
// Author(s):    main Niko Bekris, secondary Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Factory for creating entities from definitions.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <string_view>
#include <memory>

#include "Systems/GlobalEvents/IGlobalSystem.h"

// forward declarations
namespace RassEngine {
class Entity;
class UUID;
}

namespace RassEngine::Systems {

class IEntityFactory : public IGlobalSystem<IEntityFactory> {
public:
	constexpr static std::string_view DEFAULT_ENTITY_NAME = "Empty";

	virtual std::unique_ptr<Entity> Build(const std::string_view &entityName) = 0;
	virtual std::unique_ptr<Entity> Build(const std::string_view &entityName, const UUID &entityID) = 0;
	virtual std::unique_ptr<Entity> BuildEmpty(const std::string_view &entityName = DEFAULT_ENTITY_NAME) = 0;
};

};
