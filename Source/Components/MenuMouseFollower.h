// File Name:    MenuMouseFollower.h
// Author(s):    main Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Component that moves a UI element to follow the mouse.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <string>
#include <string_view>

#include "Cloneable.h"
#include "Component.h"
#include "Events/GlobalEventArgs.h"
#include "Events/GlobalEventListener.h"
#include "IEvent.h"

namespace RassEngine::Graphics {
class Texture;
}

namespace RassEngine::Components {

class MenuMouseFollower : public Cloneable<Component, MenuMouseFollower> {
public:
	MenuMouseFollower();
	MenuMouseFollower(const MenuMouseFollower &other);
	virtual ~MenuMouseFollower() override;

	virtual bool Initialize() override;
	virtual const std::string_view &NameClass() const override;
	virtual bool Read(Stream &stream) override;

private:
	bool Update(const IEvent<Events::GlobalEventArgs> *,
	            const Events::GlobalEventArgs &);

	Events::GlobalEventListener<MenuMouseFollower> onUpdateListener;
	Graphics::Texture *texture{nullptr};
	std::string texturePath{"Assets/Textures/Crosshair.png"};
	float cursorSize{32.0f};
};

}
