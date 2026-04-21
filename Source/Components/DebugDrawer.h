// File Name:    DebugDrawer.h
// Author(s):    main Boyuan Hu
// Course:       GAM541
// Project:      RASS
// Purpose:      Component for rendering debug visualization shapes.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once
#include <string_view>

#include "Component.h"
#include "Cloneable.h"
#include "Events/GlobalEventArgs.h"
#include "Events/GlobalEventListener.h"
#include "IEvent.h"
#include "Components/Sprite.h"
#include "Stream.h"
namespace RassEngine {
class Stream;
}

namespace RassEngine::Components {
class DebugDrawer : public Cloneable<Component, DebugDrawer> {
public:
	DebugDrawer();
	DebugDrawer(const DebugDrawer &other);
	virtual ~DebugDrawer() override;

	virtual bool Initialize() override;
	virtual const std::string_view &NameClass() const override;
	virtual bool Read(Stream &stream) override;

	bool showColliderBoxes{false};
	bool showTransformGizmos{false};
	bool showEntityNames{false};
	bool showSpeed{false};
	bool showMeshFrame{false};
	bool showFPS{false};
private:
	bool Update(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &);
	Events::GlobalEventListener<DebugDrawer> onUpdateListener;
	Sprite *sprite{nullptr};
};
}
