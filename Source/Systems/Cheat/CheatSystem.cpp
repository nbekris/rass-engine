// File Name:    CheatSystem.cpp
// Author(s):    main Boyuan Hu, secondary Taro Omiya, Niko Bekris, Steven Yacoub, Eric Fleegal
// Course:       GAM541
// Project:      RASS
// Purpose:      System enabling developer cheat codes.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "CheatSystem.h"

//#ifdef _DEBUG
#include <GLFW/glfw3.h>
#include <imgui.h> 
#include <initializer_list>
#include <string_view>

#include "Entity.h"
#include "Scene.h"
#include "Events/Global.h"
#include "Events/GlobalEventArgs.h"
#include "IEvent.h"
#include "Scenes/DemoScene.h"
#include "Scenes/DebugScene.h"
#include "Scenes/SerializedScene.h"
#include "Components/Camera.h"
#include "Components/HealthComponent.h"
#include "Components/ShootingController.h"
#include "Systems/Camera/ICameraSystem.h"
#include "Systems/GlobalEvents/IGlobalEventsSystem.h"
#include "Systems/Render/IRenderSystem.h"
#include "Systems/Audio/IAudioSystem.h"
#include "Systems/Logging/ILoggingSystem.h"
#include "Systems/Input/IInputSystem.h"
#include "Systems/Scene/ISceneSystem.h"



namespace RassEngine::Systems {

using namespace RassEngine::Scenes;

CheatSystem::CheatSystem(const std::initializer_list<std::string_view> &scenePaths) :
	updateListener{this, &CheatSystem::Update},
	renderListener{this, &CheatSystem::Render},
	scenePaths{} {
	// Populate scene paths
	this->scenePaths.reserve(scenePaths.size());
	for(const std::string_view &scenePath : scenePaths) {
		this->scenePaths.emplace_back(scenePath);
	}
}

CheatSystem::~CheatSystem() {
	Shutdown();
}

bool CheatSystem::Initialize() {
	if(IGlobalEventsSystem::Get() == nullptr) {
		LOG_ERROR("Cannot setup cheats: unable to bind to {}", NAMEOF(Systems::IGlobalEventsSystem));
		return false;
	} else if(IInputSystem::Get() == nullptr) {
		LOG_ERROR("Cannot setup cheats: {} is not available", NAMEOF(Systems::IInputSystem));
		return false;
	} else if(ISceneSystem::Get() == nullptr) {
		LOG_ERROR("Cannot setup cheats: {} is not available", NAMEOF(Systems::ISceneSystem));
		return false;
	}

	IGlobalEventsSystem::Get()->bind(Events::Global::Update, &updateListener);
	IGlobalEventsSystem::Get()->bind(Events::Global::Render, &renderListener);
	return true;
}

void CheatSystem::Shutdown() {
	if(IGlobalEventsSystem::Get() == nullptr) {
		return;
	}

	IGlobalEventsSystem::Get()->unbind(Events::Global::Update, &updateListener);
	IGlobalEventsSystem::Get()->unbind(Events::Global::Render, &renderListener);
}

void CheatSystem::RenderCheatMenu() {
	if(!showCheatMenu) return;
	ImGui::SetNextWindowPos(ImVec2(100, 50), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(420, 0), ImGuiCond_FirstUseEver);
	ImGui::Begin("Cheat Menu", &showCheatMenu, ImGuiWindowFlags_AlwaysAutoResize);

	ImGui::TextDisabled("Press F2 to show/hide this menu.");
	ImGui::Separator();

	// ── 1. Credits ───────────────────────────────────────────────────────
	ImGui::Text("Credits");
	ImGui::SameLine();
	if(ImGui::Button("Show Credits##btn")) {
		// TODO: verify scene name matches your credits JSON file
		ISceneSystem::Get()->SetPendingScene<SerializedScene>(std::string("Credits"));
	}
	ImGui::Separator();

	// ── 2. Fullscreen (F11) ──────────────────────────────────────────────
	const bool isFullscreen = IRenderSystem::Get() && IRenderSystem::Get()->IsFullscreen();
	ImGui::Text("Fullscreen (F11)");
	ImGui::SameLine();
	if(ImGui::Button(isFullscreen ? "Switch to Windowed##fs" : "Switch to Fullscreen##fs")) {
		if(IRenderSystem::Get()) {
			IRenderSystem::Get()->ToggleFullscreen();
		}
	}
	ImGui::Separator();

	// ── 3. Mute Music ────────────────────────────────────────────────────
	if(ImGui::Checkbox("Mute Music (SFX unaffected)", &musicMuted_)) {
		if(IAudioSystem::Get()) {
			IAudioSystem::Get()->SetMusicMuted(musicMuted_);
		}
	}
	ImGui::Separator();

	// ── 4. Skip to Next / Previous Level ────────────────────────────────
	ImGui::Text("Level Skip");
	ImGui::SameLine();
	if(ImGui::Button("Next Level##skip")) {
		auto *sceneSystem = ISceneSystem::Get();
		if(sceneSystem) {
			Scene *current = sceneSystem->GetCurrentScene();
			if(current) {
				const std::string &currentName = current->Name();
				for(size_t i = 0; i < scenePaths.size(); ++i) {
					if(scenePaths[i] == currentName && i + 1 < scenePaths.size()) {
						LOG_INFO("Cheat: skipping to next level '{}'", scenePaths[i + 1]);
						sceneSystem->SetPendingScene<SerializedScene>(scenePaths[i + 1]);
						break;
					}
				}
			}
		}
	}
	ImGui::SameLine();
	if(ImGui::Button("Previous Level##prev")) {
		auto *sceneSystem = ISceneSystem::Get();
		if(sceneSystem) {
			Scene *current = sceneSystem->GetCurrentScene();
			if(current) {
				const std::string &currentName = current->Name();
				for(size_t i = 0; i < scenePaths.size(); ++i) {
					if(scenePaths[i] == currentName && i > 0) {
						LOG_INFO("Cheat: skipping to previous level '{}'", scenePaths[i - 1]);
						sceneSystem->SetPendingScene<SerializedScene>(scenePaths[i - 1]);
						break;
					}
				}
			}
		}
	}
	ImGui::SameLine();
	if(ImGui::Button("Restart Level##skip")) {
		auto *sceneSystem = ISceneSystem::Get();
		if(sceneSystem) {
			ISceneSystem::Get()->Restart();
		}
	}
	ImGui::SameLine();
	// ── 5. Skip to End of Game ───────────────────────────────────────────
	if(ImGui::Button("Skip to End##end")) {
		if(!scenePaths.empty()) {
			LOG_INFO("Cheat: skipping to end of game '{}'", scenePaths.back());
			ISceneSystem::Get()->SetPendingScene<SerializedScene>(scenePaths.back());
		}
	}
	ImGui::Separator();

	// ── 6. Camera Zoom ───────────────────────────────────────────────────
	{
		auto *cameraSystem = ICameraSystem::Get();
		Components::Camera *cam = cameraSystem ? cameraSystem->GetActiveCamera() : nullptr;
		if(cam) {
			ImGui::Text("Camera Zoom");
			ImGui::SameLine();
			// Zoom out: lower value → sees more of the world
			if(ImGui::Button(" - ##zoomOut")) {
				cam->targetZoom *= glm::vec3(0.9f);
				cam->targetZoom = glm::max(cam->targetZoom, glm::vec3(0.1f));
			}
			ImGui::SameLine();
			// DragFloat for precise adjustment
			ImGui::SetNextItemWidth(90.0f);
			float zoomVal = cam->targetZoom.x;
			if(ImGui::DragFloat("##zoomDrag", &zoomVal, 0.01f, 0.1f, 10.0f, "%.2fx")) {
				cam->targetZoom = glm::vec3(glm::clamp(zoomVal, 0.1f, 10.0f));
			}
			ImGui::SameLine();
			// Zoom in: higher value → sees less of the world (things look larger)
			if(ImGui::Button(" + ##zoomIn")) {
				cam->targetZoom *= glm::vec3(1.1f);
			}
			ImGui::SameLine();
			if(ImGui::Button("Reset##zoomReset")) {
				cam->targetZoom = cam->basicZoom;
			}
		} else {
			ImGui::TextDisabled("Camera Zoom (no active camera)");
		}
	}
	ImGui::Separator();

	// ── 7. God Mode (invulnerable + 2x damage + rapid fire) ─────────────
	ImGui::Checkbox("God Mode  (Invulnerable + 2x Damage + Rapid Fire)", &godMode_);
	ImGui::Separator();

	// ── 8. I Lose ────────────────────────────────────────────────────────
	{
		ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.75f, 0.12f, 0.12f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.90f, 0.20f, 0.20f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.60f, 0.08f, 0.08f, 1.0f));
		if(ImGui::Button("I Lose  (Kill Player)##lose", ImVec2(-1.0f, 0.0f))) {
			auto *sceneSystem = ISceneSystem::Get();
			if(sceneSystem) {
				Entity *player = sceneSystem->FindEntity("Player");
				if(player) {
					auto *health = player->Get<Components::HealthComponent>();
					if(health) {
						LOG_INFO("Cheat: forcing player death via 'I Lose'");
						health->TakeDamage(health->GetMaxHealth() + 1);
					}
				}
			}
		}
		ImGui::PopStyleColor(3);
	}

	ImGui::Separator();
	ImGui::TextDisabled("Key shortcuts: 0=Restart  F11=Fullscreen",
		scenePaths.size());

	ImGui::End();
}

bool CheatSystem::Render(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &) {
	// ImGui calls must happen within the render frame scope (after BeginFrame, before EndFrame)
	RenderCheatMenu();
	IInputSystem::Get()->SetCursorVisible(showCheatMenu);
	return true;
}

bool CheatSystem::Update(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &) {
	// Toggle cheat menu visibility
	if(IInputSystem::Get()->IsKeyPressed(GLFW_KEY_F2)) {
		showCheatMenu = !showCheatMenu;
	}

	// F11: toggle fullscreen (grading requirement)
	if(IInputSystem::Get()->IsKeyPressed(GLFW_KEY_F11)) {
		if(IRenderSystem::Get()) {
			IRenderSystem::Get()->ToggleFullscreen();
		}
	}

	// God mode: invulnerability + rapid fire.
	// Synced every frame so state is restored correctly after scene reloads.
	{
		Entity *player = ISceneSystem::Get() ? ISceneSystem::Get()->FindEntity("Player") : nullptr;
		if(player) {
			// Invulnerability: heal to max HP every tick
			if(godMode_) {
				if(auto *health = player->Get<Components::HealthComponent>()) {
					health->Heal(health->GetMaxHealth());
				}
			}
			// Rapid fire: bypass fireCooldownTime when god mode is on.
			// Always called (not just when godMode_) so it is reset on god mode disable.
			if(auto *sc = player->Get<Components::ShootingController>()) {
				sc->SetRapidFire(godMode_);
			}
		}
	}

	// Check what key is pressed
	if(IInputSystem::Get()->IsKeyPressed(GLFW_KEY_0)) {
		// For 0, restart the current scene
		LOG_INFO("Cheat key {} pressed", GLFW_KEY_0);
		ISceneSystem::Get()->Restart();
		return true;
	}

	//// Go through all number keys besides 0
	//unsigned short key = GLFW_KEY_1;
	//for(; key <= GLFW_KEY_9; ++key) {
	//	// Halt if there are no more scenes to load
	//	unsigned short sceneIndex = key - GLFW_KEY_1;
	//	if(sceneIndex >= static_cast<unsigned short>(scenePaths.size())) {
	//		break;
	//	}

	//	// Check if the key is pressed
	//	if(IInputSystem::Get()->IsKeyPressed(key)) {
	//		// Load the scene path corresponding to this key
	//		LOG_INFO("Cheat key {} pressed", key);
	//		ISceneSystem::Get()->SetPendingScene<SerializedScene>(scenePaths[sceneIndex]);
	//		return true;
	//	}
	//}

	//// Check if the next number key is pressed
	//if((key <= GLFW_KEY_9) && IInputSystem::Get()->IsKeyPressed(key)) {
	//	// Load the demo scene
	//	LOG_INFO("Cheat key {} pressed", key);
	//	ISceneSystem::Get()->SetPendingScene<DemoScene>();
	//	return true;
	//}

	//// Check if the next number key is pressed
	//++key;
	//if((key <= GLFW_KEY_9) && IInputSystem::Get()->IsKeyPressed(key)) {
	//	// Load the debug scene
	//	LOG_INFO("Cheat key {} pressed", key);
	//	ISceneSystem::Get()->SetPendingScene<DebugScene>();
	//	return true;
	//}

	// Always succeed: no error has occurred
	return true;
}

const std::string_view &CheatSystem::NameClass() const {
	static constexpr std::string_view className = NAMEOF(Systems::CheatSystem);
	return className;
}

}
//#endif // _DEBUG
