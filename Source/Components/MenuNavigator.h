// File Name:    MenuNavigator.h
// Author(s):    main Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Component for navigating menu items with input.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <glm/vec2.hpp>
#include <string_view>
#include <vector>

#include "Cloneable.h"
#include "Component.h"
#include "Events/GlobalEventArgs.h"
#include "Events/GlobalEventListener.h"
#include "IEvent.h"

namespace RassEngine::Components {

class Button;

class MenuNavigator : public Cloneable<Component, MenuNavigator> {
public:
	MenuNavigator();
	MenuNavigator(const MenuNavigator &other);
	virtual ~MenuNavigator() override;

	virtual bool Initialize() override;
	virtual const std::string_view &NameClass() const override;
	virtual bool Read(Stream &stream) override;

private:
	bool Update(const IEvent<Events::GlobalEventArgs> *,
	            const Events::GlobalEventArgs &);
	void CollectButtons();
	void HighlightButton(int index);

	Events::GlobalEventListener<MenuNavigator> onUpdateListener;
	std::vector<Button *> buttons;
	int highlightedIndex{0};
	glm::vec2 lastMousePos{0.f, 0.f};
};

}
