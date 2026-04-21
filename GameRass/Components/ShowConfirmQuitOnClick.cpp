// File Name:    ShowConfirmQuitOnClick.cpp
// Author(s):    Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Component that shows a confirm quit prompt on the start menu.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "pch.h"
#include "ShowConfirmQuitOnClick.h"

#include <string_view>

#include <glm/vec3.hpp>
#include <imgui.h>

#include <Cloneable.h>
#include <Component.h>
#include <Components/Transform.h>
#include <Entity.h>
#include <Events/EventArgs.h>
#include <Events/Global.h>
#include <Events/UIEventIDs.h>
#include <IEvent.h>
#include <Stream.h>
#include <Systems/Entity/IEntityFactory.h>
#include <Systems/GlobalEvents/IGlobalEventsSystem.h>
#include <Systems/Logging/ILoggingSystem.h>
#include <Systems/Render/IRenderSystem.h>
#include <Systems/Scene/ISceneSystem.h>
#include <Utils.h>

using namespace RassEngine;
using namespace RassEngine::Events;
using namespace RassEngine::Systems;

namespace RassGame::Components {

ShowConfirmQuitOnClick::ShowConfirmQuitOnClick() : Cloneable<Component, ShowConfirmQuitOnClick>{}
	, onClick{this, &ShowConfirmQuitOnClick::OnClick}
	, renderListener{this, &ShowConfirmQuitOnClick::OnRender}
{}

ShowConfirmQuitOnClick::ShowConfirmQuitOnClick(const ShowConfirmQuitOnClick &other) : Cloneable<Component, ShowConfirmQuitOnClick>{other}
	, onClick{this, &ShowConfirmQuitOnClick::OnClick}
	, renderListener{this, &ShowConfirmQuitOnClick::OnRender}
{}

bool ShowConfirmQuitOnClick::Initialize() {
	if(Parent() == nullptr) {
		LOG_WARNING("{}: {} was not found", NameClass(), NAMEOF(Entity));
		return false;
	}

	Parent()->BindEvent(UIEventID::OnClick, &onClick);

	if(IGlobalEventsSystem::Get() != nullptr) {
		IGlobalEventsSystem::Get()->bind(Events::Global::Render, &renderListener);
	}

	return true;
}

ShowConfirmQuitOnClick::~ShowConfirmQuitOnClick() {
	if(Parent() != nullptr) {
		Parent()->UnbindEvent(UIEventID::OnClick, &onClick);
	}

	if(IGlobalEventsSystem::Get() != nullptr) {
		IGlobalEventsSystem::Get()->unbind(Events::Global::Render, &renderListener);
	}
}

const std::string_view &ShowConfirmQuitOnClick::NameClass() const {
	static constexpr std::string_view className = NAMEOF(Components::ShowConfirmQuitOnClick);
	return className;
}

bool ShowConfirmQuitOnClick::Read(Stream &stream) {
	if(!Component::Read(stream)) {
		return false;
	}

	return true;
}

bool ShowConfirmQuitOnClick::OnClick(const IEvent<EventArgs> *, const EventArgs &) {
	if(isShowingConfirmQuit) return true;
	isShowingConfirmQuit = true;

	auto *sceneSystem = ISceneSystem::Get();
	auto *entityFactory = IEntityFactory::Get();
	if(!sceneSystem || !entityFactory) {
		LOG_ERROR("{}: Scene or entity factory system not found", NameClass());
		return false;
	}

	// Destroy the other start menu UI entities so they are not visible.
	const char *startMenuEntityNames[] = {
		"StartMenu-LabelTitle",
		"StartMenu-ButtonStart",
		"StartMenu-ButtonOptions",
		"StartMenu-ButtonHowToPlay",
		"StartMenu-ButtonCredits",
		"MenuCursor"
	};
	for(const char *name : startMenuEntityNames) {
		Entity *entity = sceneSystem->FindEntity(name);
		if(entity) {
			entity->Destroy();
		}
	}

	// Move our own entity (the Quit button) off-screen instead of destroying it,
	// so the render listener on this component stays alive.
	auto *transform = Parent()->GetTransform();
	if(transform) {
		transform->SetPosition(glm::vec3(9999.0f, 9999.0f, 0.0f));
	}

	// Spawn the confirm quit entities.
	const char *confirmEntityNames[] = {
		"StartConfirmQuit-ButtonYes",
		"StartConfirmQuit-ButtonNo",
		"MenuCursor"
	};
	for(const char *name : confirmEntityNames) {
		auto entity = entityFactory->Build(name);
		if(entity) {
			sceneSystem->AddEntity(std::move(entity));
			spawnedEntityNames.emplace_back(name);
		} else {
			LOG_WARNING("{}: Failed to build entity '{}'", NameClass(), name);
		}
	}

	return true;
}

bool ShowConfirmQuitOnClick::OnRender(const IEvent<GlobalEventArgs> *, const GlobalEventArgs &) {
	if(!isShowingConfirmQuit) return true;

	auto *renderSystem = IRenderSystem::Get();
	if(!renderSystem) return true;

	float screenWidth = static_cast<float>(renderSystem->getScreenWidth());
	float screenHeight = static_cast<float>(renderSystem->getScreenHeight());

	ImDrawList *bgDrawList = ImGui::GetBackgroundDrawList();

	ImFont *font = ImGui::GetFont();
	const float bodyFontSize = ImGui::GetFontSize() * 1.5f;
	const ImU32 white = IM_COL32(255, 255, 255, 255);

	const char *text = "Are you sure you would like to Quit the game?";
	ImVec2 textSize = font->CalcTextSizeA(bodyFontSize, FLT_MAX, 0.0f, text);
	float x = (screenWidth - textSize.x) * 0.5f;
	bgDrawList->AddText(font, bodyFontSize, ImVec2(x, screenHeight * 0.30f), white, text);

	return true;
}

}
