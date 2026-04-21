// File Name:    AnimationController.cpp
// Author(s):    main Steven Yacoub
// Course:       GAM541
// Project:      RASS
// Purpose:      Controller component for managing animation state transitions.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"

#include "AnimationController.h"
#include "Component.h"
#include "Components/Animation.h"
#include "Components/Sprite.h"
#include "Entity.h"

namespace RassEngine::Components {

AnimationController::AnimationController() {
}

AnimationController::AnimationController(const AnimationController &other) {
	this->states = other.states;
}

AnimationController::~AnimationController() {
	states.clear();
}

bool AnimationController::Initialize() {
	return true;
}

bool AnimationController::Read(Stream& stream) {
	if(!Component::Read(stream)) {
		return false;
	}

	stream.ReadArray("Animations", [this, &stream]() {
		AnimationState animState;

		stream.Read("StateName", animState.stateName);
		stream.Read("TexturePath", animState.texturePath);
		stream.Read("NumCols", animState.numCols);
		stream.Read("NumRows", animState.numRows);
		stream.Read("FrameCount", animState.frameCount);
		stream.Read("FrameDuration", animState.frameDuration);
		stream.Read("IsLooping", animState.isLooping);

		this->states.push_back(animState);

		//stream.PopNode();
	});

	return true;
}

void AnimationController::PlayAnim(const std::string &state) {
	if(currentState == state) {
		return;
	}

	for(const AnimationState& animState : states) {
		if(animState.stateName == state) {
			currentState = state;

			auto* activeSprite = Parent()->Get<Sprite>();
			auto* activeAnim = Parent()->Get<Animation>();

			if(activeSprite && activeAnim) {
				activeSprite->SetTexture(animState.texturePath);
				activeSprite->SetCols(animState.numCols);
				activeSprite->SetRows(animState.numRows);

				activeAnim->FrameCount(animState.frameCount);
				activeAnim->FrameDuration(animState.frameDuration);
				activeAnim->FrameIndex(0.0f);
				activeAnim->IsLooping(animState.isLooping);
				activeAnim->FrameDelay(0.0f);

				activeAnim->IsRunning(true);
			}
		}
	}

	return;
}

const std::string_view &AnimationController::NameClass() const {
	static constexpr std::string_view className = NAMEOF(RassEngine::Components::AnimationController);
	return className;
}



}
