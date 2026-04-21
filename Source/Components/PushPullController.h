// File Name:    PushPullController.h
// Author(s):    main Niko Bekris, secondary Boyuan Hu
// Course:       GAM541
// Project:      RASS
// Purpose:      Component allowing the player to push or pull objects.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <string_view>
#include <vector>

#include "Component.h"
#include "Cloneable.h"
#include "Events/GlobalEventArgs.h"
#include "Events/GlobalEventListener.h"
#include "IEvent.h"

namespace RassEngine {
class Entity;
}

namespace RassEngine::Systems {
class IInputSystem;
}

namespace RassEngine::Components {

class Transform;
class PhysicsBody;
class Movement;

class PushPullController : public Cloneable<Component, PushPullController> {
public:
	PushPullController();
	PushPullController(const PushPullController &other);
	virtual ~PushPullController() override;

	virtual bool Initialize() override;
	virtual const std::string_view &NameClass() const override;
	virtual bool Read(Stream &stream) override;

	void AddNearbyPushable(Entity *entity);
	void RemoveNearbyPushable(Entity *entity);

private:
	bool OnFixedUpdate(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &);
	void ReleaseGrab();
	void LinkTextEntity();
	Events::GlobalEventListener<PushPullController> onFixedUpdateListener;

	Systems::IInputSystem *inputSystem{nullptr};
	Transform *transform{nullptr};
	PhysicsBody *physics{nullptr};
	Movement *movement{nullptr};

	std::vector<Entity *> nearbyPushables;
	std::string grabAudioFile;
	Entity *grabbedEntity{nullptr};
	Entity *text{nullptr};
	float grabOffsetX{0.0f};
	float maxGrabDistance{4.0f};
	bool entityLinked{false};
	std::string textEntityName{"TipsTextEntity2"};
};

}
