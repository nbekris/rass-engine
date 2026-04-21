// File Name:    IUiSystem.h
// Author(s):    main Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      System managing UI element layout and interaction.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <functional>
#include <string>

#include "Object.h"
#include "Systems/GlobalEvents/IGlobalSystem.h"

// Forward declaration
namespace RassEngine::Components {
class Transform;
}

namespace RassEngine::Systems {

class IUiSystem : public IGlobalSystem<IUiSystem> {
public:
	enum class State : unsigned char {
		Idle,
		Hover,
		Held,
		Disabled,
	};

	struct UiElement {
		// Required elements for rendering and interaction
		std::string name;
		std::function<const Components::Transform *(const IUiSystem &source)> getTransform;
		std::function<glm::vec2(const IUiSystem &source)> getPivot;
		std::function<void(IUiSystem &source)> render;
	};

	virtual ~IUiSystem() = default;

	virtual void AddElement(const Object *key, const UiElement &element) = 0;
	virtual void RemoveElement(const Object *key) = 0;
};

}
