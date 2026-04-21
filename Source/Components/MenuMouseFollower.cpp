// File Name:    MenuMouseFollower.cpp
// Author(s):    main Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Component that moves a UI element to follow the mouse.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "Components/MenuMouseFollower.h"

#include "Component.h"
#include "Entity.h"
#include "Events/Global.h"
#include "Events/GlobalEventArgs.h"
#include "Graphics/Texture.h"
#include "IEvent.h"
#include "Stream.h"
#include "Systems/GlobalEvents/IGlobalEventsSystem.h"
#include "Systems/Input/IInputSystem.h"
#include "Systems/Logging/ILoggingSystem.h"
#include "Systems/Render/IRenderSystem.h"
#include "Systems/Resource/IResourceSystem.h"
#include "Utils.h"

#include <imgui.h>

using namespace RassEngine::Systems;

namespace RassEngine::Components {

MenuMouseFollower::MenuMouseFollower() : Cloneable<Component, MenuMouseFollower>{}
	, onUpdateListener{this, &MenuMouseFollower::Update}
{}

MenuMouseFollower::MenuMouseFollower(const MenuMouseFollower &other) : Cloneable<Component, MenuMouseFollower>{other}
	, onUpdateListener{this, &MenuMouseFollower::Update}
	, texturePath{other.texturePath}
	, cursorSize{other.cursorSize}
{}

MenuMouseFollower::~MenuMouseFollower() {
	if(IGlobalEventsSystem::Get() == nullptr) {
		return;
	}
	IGlobalEventsSystem::Get()->unbind(Events::Global::Render, &onUpdateListener);
}

bool MenuMouseFollower::Initialize() {
	if(Parent() == nullptr) {
		LOG_WARNING("{}: {} was not found", NameClass(), NAMEOF(Entity));
		return false;
	}

	if(IGlobalEventsSystem::Get() == nullptr) {
		LOG_ERROR("{}: Global events system not found", NameClass());
		return false;
	}

	// Load the crosshair texture
	if(IResourceSystem::Get()) {
		texture = IResourceSystem::Get()->GetTexture(texturePath, true);
	}

	if(!texture) {
		LOG_WARNING("{}: Failed to load texture '{}'", NameClass(), texturePath);
	}

	IGlobalEventsSystem::Get()->bind(Events::Global::Render, &onUpdateListener);
	return true;
}

const std::string_view &MenuMouseFollower::NameClass() const {
	static constexpr std::string_view className = NAMEOF(RassEngine::Components::MenuMouseFollower);
	return className;
}

bool MenuMouseFollower::Read(Stream &stream) {
	if(!Component::Read(stream)) {
		return false;
	}

	stream.Read("TexturePath", texturePath);
	stream.Read("CursorSize", cursorSize);
	return true;
}

bool MenuMouseFollower::Update(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &) {
	if(!texture || !IInputSystem::Get() || !IRenderSystem::Get()) return true;

	float screenWidth = static_cast<float>(IRenderSystem::Get()->getScreenWidth());
	float screenHeight = static_cast<float>(IRenderSystem::Get()->getScreenHeight());

	glm::vec2 mouseNDC = IInputSystem::Get()->GetMousePositionViewport();
	mouseNDC.x = glm::clamp(mouseNDC.x, -1.0f, 1.0f);
	mouseNDC.y = glm::clamp(mouseNDC.y, -1.0f, 1.0f);

	float screenX = (mouseNDC.x + 1.0f) * 0.5f * screenWidth;
	float screenY = (1.0f - mouseNDC.y) * 0.5f * screenHeight;

	// Draw crosshair centered on cursor position using ImGui foreground draw list
	float halfSize = cursorSize * 0.5f;
	ImVec2 pMin(screenX - halfSize, screenY - halfSize);
	ImVec2 pMax(screenX + halfSize, screenY + halfSize);

	ImDrawList *drawList = ImGui::GetForegroundDrawList();
	ImTextureID texID = (ImTextureID)(intptr_t)texture->GetTextureID();
	drawList->AddImage(texID, pMin, pMax, ImVec2(0, 0), ImVec2(1, 1), IM_COL32_WHITE);

	return true;
}

}
