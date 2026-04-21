// File Name:    EntityFactory.h
// Author(s):    main Niko Bekris, secondary Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Factory for creating entities from definitions.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

#include "Entity.h"
#include "Systems/Entity/IEntityFactory.h"

// forward declarations
namespace RassEngine {
class UUID;
}

namespace RassEngine::Systems {

class EntityFactory : public IEntityFactory {
public:
	static constexpr std::string_view DEFAULT_PATH = "Entities";
	static constexpr std::string_view DEFAULT_EXTENSION = "json";

	inline EntityFactory() : IEntityFactory(), archetypes{} {}
	virtual inline ~EntityFactory() {};

	// Inherited via IEntityFactory
	bool Initialize() override;
	const std::string_view &NameClass() const override;
	void Shutdown() override;

	std::unique_ptr<Entity> Build(const std::string_view &entityName) override;
	std::unique_ptr<Entity> Build(const std::string_view &entityName, const UUID &entityID) override;

	std::unique_ptr<Entity> BuildEmpty(const std::string_view &entityName = DEFAULT_ENTITY_NAME) override;

private:
	std::unordered_map<std::string, std::unique_ptr<Entity>> archetypes;
};

}
