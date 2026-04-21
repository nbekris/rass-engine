// File Name:    UiSystem.h
// Author(s):    main Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      System managing UI element layout and interaction.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <string_view>
#include <unordered_map>

#include "Events/GlobalEventArgs.h"
#include "Events/GlobalEventListener.h"
#include "IEvent.h"
#include "IUiSystem.h"
#include "Object.h"

namespace RassEngine::Systems {

// Forward declaration
class ICameraSystem;

class UiSystem : public IUiSystem {
public:
	UiSystem();
	virtual ~UiSystem() override = default;

	// Inherited via IUiSystem
	bool Initialize() override;
	const std::string_view &NameClass() const override;
	void Shutdown() override;

	inline void AddElement(const Object *key, const UiElement &element) override {
		uiElements.emplace(key, element);
	}

	inline void RemoveElement(const Object *key) override {
		uiElements.erase(key);
	}

private:
	bool OnRender(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &);

	std::unordered_map<const Object *, UiElement> uiElements{};
	Events::GlobalEventListener<UiSystem> onRenderListener;
};

}
