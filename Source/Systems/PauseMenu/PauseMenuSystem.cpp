// File Name:    PauseMenuSystem.cpp
// Author(s):    main Niko Bekris, secondary Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      System managing the pause menu UI.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "PauseMenuSystem.h"

#include <GLFW/glfw3.h>
#include <imgui.h>

#include "Components/Camera.h"
#include "Components/Transform.h"
#include "Entity.h"
#include "Events/Global.h"
#include "Events/Window.h"
#include "Events/GlobalEventArgs.h"
#include "IEvent.h"
#include "Scene.h"
#include "Systems/Camera/ICameraSystem.h"
#include "Systems/Entity/IEntityFactory.h"
#include "Systems/GlobalEvents/IGlobalEventsSystem.h"
#include "Systems/Input/IInputSystem.h"
#include "Systems/Logging/ILoggingSystem.h"
#include "Systems/Audio/AudioOptionsUI.h"
#include "Systems/Render/IRenderSystem.h"
#include "Systems/Scene/ISceneSystem.h"
#include "Systems/Time/ITimeSystem.h"
#include "Utils.h"

namespace RassEngine::Systems {

PauseMenuSystem::PauseMenuSystem()
	: updateListener{this, &PauseMenuSystem::Update}
	, renderListener{this, &PauseMenuSystem::Render}
	, onPauseListener{this, &PauseMenuSystem::OnPause}
	, scenePaths{
		"StartMenu"
		, "OptionsMenu"
		, "HowToPlay"
		, "Credits"
		, "ConfirmQuit"
		, "DigiSplash"
		, "FMODSplash"
	}
{}

PauseMenuSystem::~PauseMenuSystem() {
	Shutdown();
}

bool PauseMenuSystem::Initialize() {
	if(IGlobalEventsSystem::Get() == nullptr) {
		LOG_ERROR("Cannot setup pause menu: unable to bind to {}", NAMEOF(Systems::IGlobalEventsSystem));
		return false;
	}

	IGlobalEventsSystem::Get()->bind(Events::Global::Update, &updateListener);
	IGlobalEventsSystem::Get()->bind(Events::Global::Render, &renderListener);
	IGlobalEventsSystem::Get()->bind(Events::Window::Minimize, &onPauseListener);
	IGlobalEventsSystem::Get()->bind(Events::Window::LoseFocus, &onPauseListener);
	return true;
}

void PauseMenuSystem::Shutdown() {
	// Restore time scale if we're shutting down while paused
	if(isPaused && ITimeSystem::Get()) {
		ITimeSystem::Get()->SetTimeScale(savedTimeScale);
		isPaused = false;
		currentSubMenu = PauseSubMenu::Main;
	}

	if(IGlobalEventsSystem::Get()) {
		IGlobalEventsSystem::Get()->unbind(Events::Global::Update, &updateListener);
		IGlobalEventsSystem::Get()->unbind(Events::Global::Render, &renderListener);
		IGlobalEventsSystem::Get()->unbind(Events::Window::Minimize, &onPauseListener);
		IGlobalEventsSystem::Get()->unbind(Events::Window::LoseFocus, &onPauseListener);
	}
}

const std::string_view &PauseMenuSystem::NameClass() const {
	static constexpr std::string_view className = NAMEOF(Systems::PauseMenuSystem);
	return className;
}

bool PauseMenuSystem::IsMenuScene() const {
	auto *sceneSystem = ISceneSystem::Get();
	if(!sceneSystem) return false;

	Scene *scene = sceneSystem->GetCurrentScene();
	if(!scene) return false;

	return scenePaths.contains(scene->Name());
}

void PauseMenuSystem::SpawnPauseEntities() {
	auto *entityFactory = IEntityFactory::Get();
	auto *sceneSystem = ISceneSystem::Get();
	if(!entityFactory || !sceneSystem) return;

	// Get the camera position so we can offset entities to be visible on screen
	glm::vec3 cameraPos{0.0f};
	auto *cameraSystem = ICameraSystem::Get();
	if(cameraSystem) {
		auto *camera = cameraSystem->GetActiveCamera();
		if(camera) {
			cameraPos = camera->position;
		}
	}

	const char *entityNames[] = {
		"PauseMenu-ButtonResume",
		"PauseMenu-ButtonOptions",
		"PauseMenu-ButtonHowToPlay",
		"PauseMenu-ButtonExit",
		"MenuCursor"
	};

	for(const char *name : entityNames) {
		auto entity = entityFactory->Build(name);
		if(entity) {
			// Offset entity position by camera position so it appears on screen
			auto *transform = entity->GetTransform();
			if(transform) {
				glm::vec3 pos = transform->GetPosition();
				pos.x += cameraPos.x;
				pos.y += cameraPos.y;
				transform->SetPosition(pos);
			}
			sceneSystem->AddEntity(std::move(entity));
			pauseEntityNames.emplace_back(name);
		} else {
			LOG_WARNING("PauseMenuSystem: Failed to build entity '{}'", name);
		}
	}
}

void PauseMenuSystem::DestroyPauseEntities() {
	auto *sceneSystem = ISceneSystem::Get();
	if(!sceneSystem) return;

	for(const auto &name : pauseEntityNames) {
		Entity *entity = sceneSystem->FindEntity(name);
		if(entity) {
			entity->Destroy();
		}
	}
	pauseEntityNames.clear();
}

void PauseMenuSystem::SpawnOptionsEntities() {
	auto *entityFactory = IEntityFactory::Get();
	auto *sceneSystem = ISceneSystem::Get();
	if(!entityFactory || !sceneSystem) return;

	// Get the camera position so we can offset entities to be visible on screen
	glm::vec3 cameraPos{0.0f};
	auto *cameraSystem = ICameraSystem::Get();
	if(cameraSystem) {
		auto *camera = cameraSystem->GetActiveCamera();
		if(camera) {
			cameraPos = camera->position;
		}
	}

	const char *entityNames[] = {
		"PauseOptions-ButtonBack",
		"MenuCursor"
	};

	for(const char *name : entityNames) {
		auto entity = entityFactory->Build(name);
		if(entity) {
			// Offset entity position by camera position so it appears on screen
			auto *transform = entity->GetTransform();
			if(transform) {
				glm::vec3 pos = transform->GetPosition();
				pos.x += cameraPos.x;
				pos.y += cameraPos.y;
				transform->SetPosition(pos);
			}
			sceneSystem->AddEntity(std::move(entity));
			optionsEntityNames.emplace_back(name);
		} else {
			LOG_WARNING("PauseMenuSystem: Failed to build entity '{}'", name);
		}
	}
}

void PauseMenuSystem::DestroyOptionsEntities() {
	auto *sceneSystem = ISceneSystem::Get();
	if(!sceneSystem) return;

	for(const auto &name : optionsEntityNames) {
		Entity *entity = sceneSystem->FindEntity(name);
		if(entity) {
			entity->Destroy();
		}
	}
	optionsEntityNames.clear();
}

void PauseMenuSystem::SpawnConfirmExitEntities() {
	auto *entityFactory = IEntityFactory::Get();
	auto *sceneSystem = ISceneSystem::Get();
	if(!entityFactory || !sceneSystem) return;

	// Get the camera position so we can offset entities to be visible on screen
	glm::vec3 cameraPos{0.0f};
	auto *cameraSystem = ICameraSystem::Get();
	if(cameraSystem) {
		auto *camera = cameraSystem->GetActiveCamera();
		if(camera) {
			cameraPos = camera->position;
		}
	}

	const char *entityNames[] = {
		"PauseConfirmExit-ButtonYes",
		"PauseConfirmExit-ButtonNo",
		"MenuCursor"
	};

	for(const char *name : entityNames) {
		auto entity = entityFactory->Build(name);
		if(entity) {
			// Offset entity position by camera position so it appears on screen
			auto *transform = entity->GetTransform();
			if(transform) {
				glm::vec3 pos = transform->GetPosition();
				pos.x += cameraPos.x;
				pos.y += cameraPos.y;
				transform->SetPosition(pos);
			}
			sceneSystem->AddEntity(std::move(entity));
			confirmExitEntityNames.emplace_back(name);
		} else {
			LOG_WARNING("PauseMenuSystem: Failed to build entity '{}'", name);
		}
	}
}

void PauseMenuSystem::DestroyConfirmExitEntities() {
	auto *sceneSystem = ISceneSystem::Get();
	if(!sceneSystem) return;

	for(const auto &name : confirmExitEntityNames) {
		Entity *entity = sceneSystem->FindEntity(name);
		if(entity) {
			entity->Destroy();
		}
	}
	confirmExitEntityNames.clear();
}

void PauseMenuSystem::SpawnHowToPlayEntities() {
	auto *entityFactory = IEntityFactory::Get();
	auto *sceneSystem = ISceneSystem::Get();
	if(!entityFactory || !sceneSystem) return;

	glm::vec3 cameraPos{0.0f};
	auto *cameraSystem = ICameraSystem::Get();
	if(cameraSystem) {
		auto *camera = cameraSystem->GetActiveCamera();
		if(camera) {
			cameraPos = camera->position;
		}
	}

	const char *entityNames[] = {
		"PauseHowToPlayBoard",
		"PauseHowToPlay-ButtonBack",
		"MenuCursor"
	};

	for(const char *name : entityNames) {
		auto entity = entityFactory->Build(name);
		if(entity) {
			auto *transform = entity->GetTransform();
			if(transform) {
				glm::vec3 pos = transform->GetPosition();
				pos.x += cameraPos.x;
				pos.y += cameraPos.y;
				transform->SetPosition(pos);
			}
			sceneSystem->AddEntity(std::move(entity));
			howToPlayEntityNames.emplace_back(name);
		} else {
			LOG_WARNING("PauseMenuSystem: Failed to build entity '{}'", name);
		}
	}
}

void PauseMenuSystem::DestroyHowToPlayEntities() {
	auto *sceneSystem = ISceneSystem::Get();
	if(!sceneSystem) return;

	for(const auto &name : howToPlayEntityNames) {
		Entity *entity = sceneSystem->FindEntity(name);
		if(entity) {
			entity->Destroy();
		}
	}
	howToPlayEntityNames.clear();
}

void PauseMenuSystem::Pause() {
	if(isPaused) return;
	isPaused = true;

	if(ITimeSystem::Get()) {
		savedTimeScale = ITimeSystem::Get()->GetTimeScale();
		ITimeSystem::Get()->SetTimeScale(0.0f);
	}

	SpawnPauseEntities();
}

void PauseMenuSystem::Resume() {
	if(!isPaused) return;
	isPaused = false;

	if(currentSubMenu == PauseSubMenu::Options) {
		DestroyOptionsEntities();
	} else if(currentSubMenu == PauseSubMenu::HowToPlay) {
		DestroyHowToPlayEntities();
	} else if(currentSubMenu == PauseSubMenu::ConfirmExit) {
		DestroyConfirmExitEntities();
	} else {
		DestroyPauseEntities();
	}
	currentSubMenu = PauseSubMenu::Main;

	if(ITimeSystem::Get()) {
		ITimeSystem::Get()->SetTimeScale(savedTimeScale);
	}
}

void PauseMenuSystem::ShowOptions() {
	if(!isPaused) return;

	DestroyPauseEntities();
	SpawnOptionsEntities();
	currentSubMenu = PauseSubMenu::Options;
}

void PauseMenuSystem::ShowHowToPlay() {
	if(!isPaused) return;

	DestroyPauseEntities();
	SpawnHowToPlayEntities();
	currentSubMenu = PauseSubMenu::HowToPlay;
}

void PauseMenuSystem::BackToPause() {
	if(!isPaused) return;

	if(currentSubMenu == PauseSubMenu::HowToPlay) {
		DestroyHowToPlayEntities();
	} else if(currentSubMenu == PauseSubMenu::Options) {
		DestroyOptionsEntities();
	} else if(currentSubMenu == PauseSubMenu::ConfirmExit) {
		DestroyConfirmExitEntities();
	}
	SpawnPauseEntities();
	currentSubMenu = PauseSubMenu::Main;
}

void PauseMenuSystem::ShowConfirmExit() {
	if(!isPaused) return;

	DestroyPauseEntities();
	SpawnConfirmExitEntities();
	currentSubMenu = PauseSubMenu::ConfirmExit;
}

bool PauseMenuSystem::Update(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &) {
	if(!IInputSystem::Get()) return true;

	// Don't allow pause on menu screens
	if(IsMenuScene()) {
		if(isPaused) {
			// Scene has already changed — old pause entities are gone.
			// Just reset state and restore time scale without destroying entities,
			// otherwise we'd destroy the menu scene's own MenuCursor by name.
			isPaused = false;
			pauseEntityNames.clear();
			optionsEntityNames.clear();
			howToPlayEntityNames.clear();
			currentSubMenu = PauseSubMenu::Main;
			if(ITimeSystem::Get()) {
				ITimeSystem::Get()->SetTimeScale(savedTimeScale);
			}
		}
		return true;
	}

	// Toggle pause with ESC
	if(IInputSystem::Get()->IsKeyPressed(GLFW_KEY_ESCAPE)) {
		if(isPaused && (currentSubMenu == PauseSubMenu::Options || currentSubMenu == PauseSubMenu::HowToPlay || currentSubMenu == PauseSubMenu::ConfirmExit)) {
			BackToPause();
		} else if(isPaused) {
			Resume();
		} else {
			Pause();
		}
		return true;
	}
	return true;
}

bool PauseMenuSystem::Render(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &) {
	if(!isPaused) return true;

	auto *renderSystem = IRenderSystem::Get();
	if(!renderSystem) return true;

	float screenWidth = static_cast<float>(renderSystem->getScreenWidth());
	float screenHeight = static_cast<float>(renderSystem->getScreenHeight());

	ImDrawList *bgDrawList = ImGui::GetBackgroundDrawList();

	// Draw semi-transparent dark overlay over the entire screen.
	// Skipped for the How To Play submenu so the board sprite reads clearly.
	if(currentSubMenu != PauseSubMenu::HowToPlay) {
		bgDrawList->AddRectFilled(
			ImVec2(0, 0),
			ImVec2(screenWidth, screenHeight),
			IM_COL32(0, 0, 0, 150)
		);
	}

	// Draw label text on top of the overlay using the same draw list.
	// Items are rendered in submission order, so text added after the rect
	// appears above it — but below all ImGui windows (buttons).
	ImFont *font = ImGui::GetFont();
	const float titleFontSize = ImGui::GetFontSize() * 2.0f;
	const float bodyFontSize = ImGui::GetFontSize() * 1.5f;
	const ImU32 white = IM_COL32(255, 255, 255, 255);

	auto drawCenteredText = [&](const char *text, float fontSize, float y) {
		ImVec2 textSize = font->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, text);
		float x = (screenWidth - textSize.x) * 0.5f;
		bgDrawList->AddText(font, fontSize, ImVec2(x, y), white, text);
	};

	if(currentSubMenu == PauseSubMenu::Main) {
		drawCenteredText("PAUSED", titleFontSize, screenHeight * 0.25f);
	} else if(currentSubMenu == PauseSubMenu::Options) {
		drawCenteredText("OPTIONS", titleFontSize, screenHeight * 0.25f);
		RenderAudioSliders();
	} else if(currentSubMenu == PauseSubMenu::ConfirmExit) {
		drawCenteredText("Would you like to go back to the Start Menu?", bodyFontSize, screenHeight * 0.30f);
		drawCenteredText("All progress will be lost.", bodyFontSize, screenHeight * 0.40f);
	}

	return true;
}

bool PauseMenuSystem::OnPause(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &) {
	// Don't allow pause on menu screens
	if(IsMenuScene()) {
		return true;
	}

	// If we lose focus or minimize while not already paused, pause the game
	if(!isPaused) {
		Pause();
	}
	return true;
}

}
