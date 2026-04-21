// File Name:    SinTranslate.h
// Author(s):    main Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Behavior that translates an entity using a sine wave.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <numbers>
#include <string_view>

#include "Component.h"
#include "Cloneable.h"
#include "Events/GlobalEventListener.h"
#include "IEvent.h"
#include "Stream.h"

namespace RassEngine::Events {
struct GlobalEventArgs;
}

namespace RassEngine::Components::Behaviors {

class SinTranslate : public Cloneable<Component, SinTranslate> {
public:
	SinTranslate();
	SinTranslate(const SinTranslate &other);
	virtual ~SinTranslate() override;

	virtual bool Initialize() override;
	virtual const std::string_view &NameClass() const override;
	virtual bool Read(Stream &stream) override;
private:
	bool Update(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &);
private:
	Events::GlobalEventListener<SinTranslate> onUpdateListener;
	glm::vec3 originalPosition{1.0f};
	glm::vec3 magnitude{std::numbers::pi_v<float>};
	glm::vec3 interval{1.0f};
	glm::vec3 offset{0.0f};
};

}
