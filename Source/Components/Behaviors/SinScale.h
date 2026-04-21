// File Name:    SinScale.h
// Author(s):    main Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Behavior that scales an entity using a sine wave.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <glm/vec3.hpp>
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

class SinScale : public Cloneable<Component, SinScale> {
public:
	SinScale();
	SinScale(const SinScale &other);
	virtual ~SinScale() override;

	virtual bool Initialize() override;
	virtual const std::string_view &NameClass() const override;
	virtual bool Read(Stream &stream) override;
private:
	bool Update(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &);
private:
	Events::GlobalEventListener<SinScale> onUpdateListener;
	glm::vec3 originalScale{1.0f, 1.0f, 1.0f};
	float magnitude{1.0f};
	float interval{1.0f};
	float offset{0.0f};
};

}
