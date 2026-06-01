// File Name:    SpriteFader.h
// Author(s):    main Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Component that fades a sprite alpha in or out.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once
#include <string_view>

#include "Cloneable.h"
#include "Component.h"
#include "Events/GlobalEventArgs.h"
#include "Events/GlobalEventListener.h"

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

private:
	bool OnUpdate(const RassEngine::IEvent<RassEngine::Events::GlobalEventArgs> *, const RassEngine::Events::GlobalEventArgs &args);
	Sprite *GetSprite() const;

private:
	float fadeDuration{2.0f};
	float currentTime{0.0f};
	mutable Sprite *spriteCache{nullptr};

	RassEngine::Events::GlobalEventListener<SpriteFader> onUpdateListener;
};
}

