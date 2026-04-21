// File Name:    ParallaxController.cpp
// Author(s):    main Boyuan Hu
// Course:       GAM541
// Project:      RASS
// Purpose:      Component implementing parallax scrolling for backgrounds.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "pch.h"
#include "ParallaxController.h"

#include <Cloneable.h>
#include <Component.h>
#include <Entity.h>
#include <Events/Global.h>
#include <Events/GlobalEventArgs.h>
#include <Components/Sprite.h>
//#include <Components/Transform.h> 
#include <glm/vec3.hpp>
#include <IEvent.h>
#include <Stream.h>
#include <Systems/GlobalEvents/IGlobalEventsSystem.h>
#include <Systems/Logging/ILoggingSystem.h>
#include <Systems/Camera/ICameraSystem.h>
#include <Utils.h>


using namespace RassEngine;
using namespace RassEngine::Systems;

namespace RassGame::Components {

ParallaxController::ParallaxController()
	: Cloneable<Component, ParallaxController>{}
	, onUpdate{this, &ParallaxController::OnUpdate}
{}

ParallaxController::ParallaxController(const ParallaxController &other)
	: Cloneable<Component, ParallaxController>{other}
	, onUpdate{this, &ParallaxController::OnUpdate}
	, parallaxFactor{other.parallaxFactor}
{}

ParallaxController::~ParallaxController() {
	if(IGlobalEventsSystem::Get()) {
		IGlobalEventsSystem::Get()->unbind(Events::Global::Update, &onUpdate);
	}
}

bool ParallaxController::Initialize() {
	if(Parent() == nullptr) {
		LOG_WARNING("{}: Entity not found", NameClass());
		return false;
	}

	if(IGlobalEventsSystem::Get()) {
		IGlobalEventsSystem::Get()->bind(Events::Global::Update, &onUpdate);
	}
	return true;
}

const std::string_view &ParallaxController::NameClass() const {
	static constexpr std::string_view className = NAMEOF(Components::ParallaxController);
	return className;
}

bool ParallaxController::Read(Stream &stream) {
	if(!Component::Read(stream)) return false;
	stream.ReadVec3("ParallaxFactor", parallaxFactor);
	return true;
}

bool ParallaxController::OnUpdate(
	const IEvent<Events::GlobalEventArgs> *,
	const Events::GlobalEventArgs &) {
	if(!entitiesLinked_) {
		camera = FindCamera();
		bgSprite = FindBackgroundSprite();
		if(camera!=nullptr && bgSprite!=nullptr) {
			entitiesLinked_ = true;
			initialCameraPos = camera->position;	
		}
	}
	SetupParallax();
	return true;
}
void ParallaxController::SetupParallax() {
	if(!camera || !bgSprite) {
		return;
	}
	// Get camera position and window size
	//auto [windowPos, windowSize] = ICameraSystem::Get()->GetCameraWindowPosSize();
	//camera
	// Calculate parallax position based on camera position and parallax factor
	//glm::vec3 parallaxPos =(windowPos + camera->offset )* parallaxFactor;
	//glm::vec3 parallaxPos = (windowPos + camera->aimOffset - initialCameraPos) * parallaxFactor;
	glm::vec3 parallaxPos =  (camera->position + camera->aimOffset - initialCameraPos) * parallaxFactor;
	// Update background sprite position to create parallax effect
	bgSprite->SetOffset(parallaxPos.x, parallaxPos.y);
}

RassEngine::Components::Camera *ParallaxController::FindCamera() const {
	ICameraSystem *cameraSystem = ICameraSystem::Get();
	if(!cameraSystem) {
		LOG_WARNING("{}: Unable to retrieve {}", NameClass(), NAMEOF(ICameraSystem));
		return nullptr;
	}

	return cameraSystem->GetActiveCamera();
}

RassEngine::Components::Sprite *ParallaxController::FindBackgroundSprite() const {
	return Parent()->Get<RassEngine::Components::Sprite>();
}
}
