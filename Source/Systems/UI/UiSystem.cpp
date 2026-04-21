// File Name:    UiSystem.cpp
// Author(s):    main Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      System managing UI element layout and interaction.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "UiSystem.h"

#include <glm/vec2.hpp>
#include <imgui.h>
#include <string_view>

#include "Components/Transform.h"
#include "Events/Global.h"
#include "Events/GlobalEventArgs.h"
#include "IEvent.h"
#include "Systems/Camera/ICameraSystem.h"
#include "Systems/GlobalEvents/IGlobalEventsSystem.h"
#include "Systems/Logging/ILoggingSystem.h"
#include "Utils.h"
#include "UUID.h"

namespace RassEngine::Systems {

UiSystem::UiSystem()
	: onRenderListener{this, &UiSystem::OnRender} {}

bool UiSystem::Initialize() {
	uiElements.clear();

	if(IGlobalEventsSystem::Get() == nullptr) {
		LOG_ERROR("{}: Global system not found when initializing UI system", NameClass());
		return false;
	}

	IGlobalEventsSystem::Get()->bind(Events::Global::Render, &onRenderListener);
	return true;
}

void UiSystem::Shutdown() {
	uiElements.clear();

	if(IGlobalEventsSystem::Get() == nullptr) {
		LOG_ERROR("{}: Global system not found when initializing UI system", NameClass());
		return;
	}

	IGlobalEventsSystem::Get()->unbind(Events::Global::Render, &onRenderListener);
}

bool UiSystem::OnRender(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &) {
	static const ImVec2 ZERO{0.f, 0.f};

	if(ICameraSystem::Get() == nullptr) {
		LOG_ERROR("{}: Camera system not found when updating UI system", NameClass());
		return false;
	}

	for(const auto &[key, element] : uiElements) {
		// Make sure element has most functions assigned
		if(element.getTransform == nullptr || element.getPivot == nullptr || element.render == nullptr) {
			LOG_ERROR("{}: UI element with key {} is missing required functions", NameClass(), static_cast<UUID::Type>(key->GetID()));
			continue;
		}

		// Get control position
		glm::vec2 pxPos = ICameraSystem::Get()->WorldToScreenPixels(
			element.getTransform(*this)->GetPosition()
		);
		pxPos.x = std::isnan(pxPos.x) ? 0.f : pxPos.x;
		pxPos.y = std::isnan(pxPos.y) ? 0.f : pxPos.y;
		glm::vec2 pivot = element.getPivot(*this);

		// Setup window
		ImGui::SetNextWindowPos(ImVec2(pxPos.x, pxPos.y), ImGuiCond_Always, ImVec2(pivot.x, pivot.y));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ZERO);
		ImGui::Begin(element.name.c_str(), nullptr
			, ImGuiWindowFlags_AlwaysAutoResize
			| ImGuiWindowFlags_NoDecoration);

		// Render the element
		ImGui::SetWindowFontScale(2.0f);
		element.render(*this);

		// Finish rendering
		ImGui::End();
		ImGui::PopStyleVar();
	}

	return true;
}

const std::string_view &UiSystem::NameClass() const {
	static constexpr std::string_view className = NAMEOF(UiSystem);
	return className;
}

}
