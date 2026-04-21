// File Name:    Button.cpp
// Author(s):    main Taro Omiya, secondary Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      UI button component handling click interaction.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "Components/Button.h"

#include <imgui.h>
#include <string>
#include <string_view>

#include "Cloneable.h"
#include "Component.h"
#include "Entity.h"
#include "Events/UIEventIDs.h"
#include "Stream.h"
#include "Systems/Logging/ILoggingSystem.h"
#include "Systems/UI/IUiSystem.h"
#include "Utils.h"

namespace RassEngine::Components {
using namespace RassEngine::Systems;

Button::Button()
	: Cloneable<Component, Button>{}
	, uiElement{} {
}

Button::Button(const Button &other)
	: Cloneable<Component, Button>{other}
	, text{other.text}
	, size{other.size}
	, pivot{other.pivot}
	, isInteractable{other.isInteractable}
	, isHighlighted{false}
	, uiElement{} {
}

Button::~Button() {
	if(IUiSystem::Get() != nullptr) {
		IUiSystem::Get()->RemoveElement(this);
	}
}

bool Button::Initialize() {
	// Using UUID as name should make each button name unique
	uiElement.name = std::to_string(static_cast<UUID::Type>(GetID()));
	uiElement.getTransform = [this] (const IUiSystem &source) {
		return Parent()->GetTransform();
	};
	uiElement.getPivot = [this] (const IUiSystem &source) {
		return pivot;
	};
	uiElement.render = [this] (IUiSystem &source) {
		ImGui::BeginDisabled(!isInteractable);
		if(isHighlighted) {
			ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered));
		}
		if(ImGui::Button(this->text.c_str(), ImVec2(size.x, size.y))) {
			Parent()->DispatchEntityEvent(Events::UIEventID::OnClick, EventArgs{*this});
		}
		if(isHighlighted) {
			ImGui::PopStyleColor();
		}
		ImGui::EndDisabled();
	};

	// Add this element into the UI System
	if(IUiSystem::Get() == nullptr) {
		LOG_ERROR("{}: UI system not found when initializing Button component", NameClass());
		return false;
	}
	IUiSystem::Get()->AddElement(this, uiElement);
	return true;
}

bool Button::Read(Stream &stream) {
	if(!Component::Read(stream)) {
		return false;
	}

	stream.Read("Text", text);
	stream.ReadVec2("Size", size);
	stream.ReadVec2("Pivot", pivot);
	stream.Read("Interactable", isInteractable);
	return true;
}

void Button::SetText(const std::string &text) {
	this->text = text;
}

void Button::SetInteractable(bool flag) {
	isInteractable = flag;
}

void Button::SetHighlighted(bool flag) {
	isHighlighted = flag;
}

const std::string_view &Button::NameClass() const {
	static constexpr std::string_view className = NAMEOF(RassEngine::Components::Button);
	return className;
}

}
