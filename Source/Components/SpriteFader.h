// File Name:    SpriteFader.h
// Author(s):    main Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Component that fades a sprite alpha in or out.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <glm/vec3.hpp>
#include <string_view>

#include "Cloneable.h"
#include "Component.h"
#include "Events/GlobalEventArgs.h"
#include "Events/GlobalEventListener.h"
#include "TweenCurve.h"

namespace RassEngine::Components {
// Forward declaration
class Sprite;

// Actual class definition
class SpriteFader : public Cloneable<Component, SpriteFader> {
public:
	SpriteFader();
	SpriteFader(const SpriteFader &other);
	virtual ~SpriteFader() override;

	virtual bool Initialize() override;
	virtual const std::string_view &NameClass() const override;
	virtual bool Read(Stream &stream) override;

	bool Show(const glm::vec3 &color, const TweenCurve &curve);

private:
	bool OnUpdate(const RassEngine::IEvent<RassEngine::Events::GlobalEventArgs> *, const RassEngine::Events::GlobalEventArgs &args);
	Sprite *GetSprite() const;
	inline void UpdateLinearCurve() {
		curve.Set(0.0f, {TweenCurve::KeyFrame{TweenCurve::Type::Linear, fadeDuration, 1.0f}});
	}

private:
	float fadeDuration{2.0f};
	float currentTime{0.0f};
	TweenCurve curve{};
	bool autoDisable{true};
	mutable Sprite *spriteCache{nullptr};

	RassEngine::Events::GlobalEventListener<SpriteFader> onUpdateListener;
};
}

