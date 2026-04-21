// File Name:    MenuNavigator.cpp
// Author(s):    main Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Component for navigating menu items with input.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "Components/MenuNavigator.h"

#include <algorithm>
#include <GLFW/glfw3.h>

#include "Component.h"
#include "Components/Button.h"
#include "Components/Transform.h"
#include "Entity.h"
#include "Events/Global.h"
#include "Events/GlobalEventArgs.h"
#include "Events/UIEventIDs.h"
#include "IEvent.h"
#include "Stream.h"
#include "Systems/GlobalEvents/IGlobalEventsSystem.h"
#include "Systems/Input/IInputSystem.h"
#include "Systems/Logging/ILoggingSystem.h"
#include "Systems/Scene/ISceneSystem.h"
#include "Utils.h"

using namespace RassEngine::Systems;

namespace RassEngine::Components {

MenuNavigator::MenuNavigator() : Cloneable<Component, MenuNavigator>{}
	, onUpdateListener{this, &MenuNavigator::Update}
{}

MenuNavigator::MenuNavigator(const MenuNavigator &other) : Cloneable<Component, MenuNavigator>{other}
	, onUpdateListener{this, &MenuNavigator::Update}
{}

MenuNavigator::~MenuNavigator() {
	// Clear highlights on all tracked buttons
	for(auto *btn : buttons) {
		if(btn) {
			btn->SetHighlighted(false);
		}
	}

	if(IGlobalEventsSystem::Get() == nullptr) {
		return;
	}
	IGlobalEventsSystem::Get()->unbind(Events::Global::Update, &onUpdateListener);
}

bool MenuNavigator::Initialize() {
	if(Parent() == nullptr) {
		LOG_WARNING("{}: {} was not found", NameClass(), NAMEOF(Entity));
		return false;
	}

	if(IGlobalEventsSystem::Get() == nullptr) {
		LOG_ERROR("{}: Global events system not found", NameClass());
		return false;
	}

	IGlobalEventsSystem::Get()->bind(Events::Global::Update, &onUpdateListener);
	return true;
}

const std::string_view &MenuNavigator::NameClass() const {
	static constexpr std::string_view className = NAMEOF(RassEngine::Components::MenuNavigator);
	return className;
}

bool MenuNavigator::Read(Stream &stream) {
	if(!Component::Read(stream)) {
		return false;
	}
	return true;
}

void MenuNavigator::CollectButtons() {
	// Clear old highlights
	for(auto *btn : buttons) {
		if(btn) {
			btn->SetHighlighted(false);
		}
	}
	buttons.clear();

	auto *sceneSystem = ISceneSystem::Get();
	if(!sceneSystem) return;

	// Collect all interactable buttons in the scene
	sceneSystem->ForEachActiveEntity([this](const Entity *entity) {
		Button *btn = entity->Get<Button>();
		if(btn && btn->IsInteractable()) {
			buttons.push_back(btn);
		}
	});

	// Sort by Y position descending (higher Y = higher on screen = first in nav order)
	std::sort(buttons.begin(), buttons.end(), [](const Button *a, const Button *b) {
		auto *transformA = a->Parent()->GetTransform();
		auto *transformB = b->Parent()->GetTransform();
		return transformA->GetPosition().y > transformB->GetPosition().y;
	});
}

void MenuNavigator::HighlightButton(int index) {
	// Clear old highlight
	if(highlightedIndex >= 0 && highlightedIndex < static_cast<int>(buttons.size())) {
		buttons[highlightedIndex]->SetHighlighted(false);
	}

	highlightedIndex = index;

	// Set new highlight
	if(highlightedIndex >= 0 && highlightedIndex < static_cast<int>(buttons.size())) {
		buttons[highlightedIndex]->SetHighlighted(true);
	}
}

bool MenuNavigator::Update(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &) {
	auto *input = IInputSystem::Get();
	if(!input) return true;

	// Re-collect buttons each frame to handle dynamic spawning/destruction
	size_t oldCount = buttons.size();
	CollectButtons();

	if(buttons.empty()) return true;

	// If button list changed, reset to first button
	if(buttons.size() != oldCount) {
		highlightedIndex = 0;
	}

	// Clamp index in case buttons were removed
	if(highlightedIndex >= static_cast<int>(buttons.size())) {
		highlightedIndex = 0;
	}

	// Detect mouse movement — clear keyboard highlight when mouse moves
	glm::vec2 currentMousePos = input->GetMousePositionViewport();
	if(currentMousePos != lastMousePos) {
		if(highlightedIndex >= 0) {
			HighlightButton(-1);
		}
		lastMousePos = currentMousePos;
	}

	// Navigate with Up/Down arrows
	if(input->IsKeyPressed(GLFW_KEY_DOWN) || input->IsKeyPressed(GLFW_KEY_UP)) {
		// If no button is highlighted (mouse was used), re-highlight the first button
		if(highlightedIndex < 0) {
			HighlightButton(0);
		} else if(input->IsKeyPressed(GLFW_KEY_DOWN)) {
			int next = (highlightedIndex + 1) % static_cast<int>(buttons.size());
			HighlightButton(next);
		} else {
			int prev = (highlightedIndex - 1 + static_cast<int>(buttons.size())) % static_cast<int>(buttons.size());
			HighlightButton(prev);
		}
	}

	// Activate with Enter or Space
	if(input->IsKeyPressed(GLFW_KEY_ENTER) || input->IsKeyPressed(GLFW_KEY_SPACE)) {
		if(highlightedIndex >= 0 && highlightedIndex < static_cast<int>(buttons.size())) {
			Button *btn = buttons[highlightedIndex];
			if(btn->IsInteractable()) {
				btn->Parent()->DispatchEntityEvent(Events::UIEventID::OnClick, Button::EventArgs{*btn});
			}
		}
	}

	// Apply highlight to current button (only if one is selected)
	if(highlightedIndex >= 0) {
		HighlightButton(highlightedIndex);
	}

	return true;
}

}
