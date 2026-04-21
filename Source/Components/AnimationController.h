// File Name:    AnimationController.h
// Author(s):    main Steven Yacoub
// Course:       GAM541
// Project:      RASS
// Purpose:      Controller component for managing animation state transitions.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include "Component.h"
#include "Cloneable.h"
#include "Animation.h"
#include <string_view>
#include <string>

// Forward declaring json
namespace RassEngine {
	class Stream;
}

namespace RassEngine::Components {

	struct AnimationState {
		std::string stateName{""};

		std::string texturePath{""};
		unsigned int numCols{1};
		unsigned int numRows{1};

		unsigned int frameCount{1};
		float frameDuration{0.1f};
		bool isLooping{true};
	};

	class AnimationController : public Cloneable<Component, AnimationController> {
	public:
		AnimationController();
		AnimationController(const AnimationController& other);
		virtual ~AnimationController() override;

		virtual bool Initialize() override;
		virtual const std::string_view &NameClass() const override;

		virtual bool Read(Stream& stream) override;

		void PlayAnim(const std::string& state);

	private:
		std::vector<AnimationState> states;
		std::string currentState;
	};
}
