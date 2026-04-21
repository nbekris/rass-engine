// File Name:    SinRotate.h
// Author(s):    main Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Behavior that rotates an entity using a sine wave.
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

class SinRotate : public Cloneable<Component, SinRotate> {
public:
	SinRotate();
	SinRotate(const SinRotate&other);
	virtual ~SinRotate() override;

	virtual bool Initialize() override;
	virtual const std::string_view &NameClass() const override;
	virtual bool Read(Stream &stream) override;
private:
	bool Update(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &);
private:
	Events::GlobalEventListener<SinRotate> onUpdateListener;
	float originalRotation{1.0f};	
	float magnitude{std::numbers::pi_v<float>};
	float interval{1.0f};
	float offset{0.0f};
};

}
