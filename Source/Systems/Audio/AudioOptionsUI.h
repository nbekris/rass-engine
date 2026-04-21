// File Name:    AudioOptionsUI.h
// Author(s):    main Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      UI for configuring audio options.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <imgui.h>

#include "Systems/Audio/IAudioSystem.h"
#include "Systems/Render/IRenderSystem.h"

namespace RassEngine::Systems {

inline void RenderAudioSliders() {
	auto *audio = IAudioSystem::Get();
	auto *render = IRenderSystem::Get();
	if(!audio || !render) return;

	float screenWidth = static_cast<float>(render->getScreenWidth());
	float screenHeight = static_cast<float>(render->getScreenHeight());

	float windowWidth = 350.0f;
	float windowX = (screenWidth - windowWidth) * 0.5f;
	float windowY = screenHeight * 0.35f;

	float fontSize = ImGui::GetFontSize();
	float scaledFontSize = fontSize * 1.5f;
	float fontScale = scaledFontSize / fontSize;

	ImGui::SetNextWindowPos(ImVec2(windowX, windowY));
	ImGui::SetNextWindowSize(ImVec2(windowWidth, 0));
	ImGui::Begin("##AudioOptions", nullptr,
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoSavedSettings);

	float musicVol = audio->GetMusicVolume();
	float sfxVol = audio->GetSFXVolume();

	ImVec4 buttonColor = ImGui::GetStyleColorVec4(ImGuiCol_Button);
	ImVec4 boxColor(buttonColor.x * 0.4f, buttonColor.y * 0.4f, buttonColor.z * 0.4f, buttonColor.w);
	ImVec2 padding(10.0f, 6.0f);

	ImGui::SetWindowFontScale(fontScale);

	// Music Volume label in a darker box
	{
		ImVec2 textSize = ImGui::CalcTextSize("Music Volume");
		ImVec2 cursorPos = ImGui::GetCursorScreenPos();
		ImGui::GetWindowDrawList()->AddRectFilled(
			cursorPos,
			ImVec2(cursorPos.x + textSize.x + padding.x * 2.0f, cursorPos.y + textSize.y + padding.y * 2.0f),
			ImGui::ColorConvertFloat4ToU32(boxColor),
			4.0f
		);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + padding.x);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + padding.y);
		ImGui::TextUnformatted("Music Volume");
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + padding.y);
	}
	if(ImGui::SliderFloat("##MusicVolume", &musicVol, 0.0f, 1.0f, "%.2f")) {
		audio->SetMusicVolume(musicVol);
	}

	ImGui::Spacing();

	// SFX Volume label in a darker box
	{
		ImVec2 textSize = ImGui::CalcTextSize("SFX Volume");
		ImVec2 cursorPos = ImGui::GetCursorScreenPos();
		ImGui::GetWindowDrawList()->AddRectFilled(
			cursorPos,
			ImVec2(cursorPos.x + textSize.x + padding.x * 2.0f, cursorPos.y + textSize.y + padding.y * 2.0f),
			ImGui::ColorConvertFloat4ToU32(boxColor),
			4.0f
		);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + padding.x);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + padding.y);
		ImGui::TextUnformatted("SFX Volume");
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + padding.y);
	}
	if(ImGui::SliderFloat("##SFXVolume", &sfxVol, 0.0f, 1.0f, "%.2f")) {
		audio->SetSFXVolume(sfxVol);
	}

	ImGui::SetWindowFontScale(1.0f);

	ImGui::End();
}

}
