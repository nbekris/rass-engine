// File Name:    ParallaxController.h
// Author(s):    main Boyuan Hu
// Course:       GAM541
// Project:      RASS
// Purpose:      Component implementing parallax scrolling for backgrounds.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <climits>
#include <Cloneable.h>
#include <Component.h>
#include <Components/Camera.h>
#include <Components/Sprite.h>
#include "Systems/Camera/ICameraSystem.h"
#include <Events/EventListenerMethod.h>
#include <Events/GlobalEventArgs.h>
#include <IEvent.h>
#include <Stream.h>
#include <string_view>
#include <UUID.h>

namespace RassGame::Components {

class ParallaxController : public RassEngine::Cloneable<RassEngine::Component, ParallaxController> {
public:
	ParallaxController();
	ParallaxController(const ParallaxController &other);
	virtual ~ParallaxController() override;
	virtual bool Initialize() override;
	virtual const std::string_view &NameClass() const override;
	virtual bool Read(RassEngine::Stream &stream) override;

private:
	bool OnUpdate(
		const RassEngine::IEvent<RassEngine::Events::GlobalEventArgs> *,
		const RassEngine::Events::GlobalEventArgs &);
	RassEngine::Components::Sprite *FindBackgroundSprite() const;
	RassEngine::Components::Camera *FindCamera() const;
	void SetupParallax();
private:

	//bool useVerticalMirrorEffect{true};// If true,use  GL_MIRRORED_REPEAT for background texture
	glm::vec3 parallaxFactor{0.05f, 0.05f, 0.0f};
	RassEngine::Components::Camera *camera{nullptr};
	RassEngine::Components::Sprite *bgSprite{nullptr};
	glm::vec3 initialCameraPos{0.0f};
	bool entitiesLinked_{false};
	RassEngine::Events::EventListenerMethod<ParallaxController, RassEngine::Events::GlobalEventArgs> onUpdate;
};

} // namespace RassGame::Components
