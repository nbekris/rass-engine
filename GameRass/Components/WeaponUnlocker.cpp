// File Name:    WeaponUnlocker.cpp
// Author(s):    main Boyuan Hu
// Course:       GAM541
// Project:      RASS
// Purpose:      Component that unlocks a weapon upon player interaction.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "WeaponUnlocker.h"

//#include "BulletBehavior.h"
#include "Cloneable.h"
#include "Component.h"
#include "Components/Camera.h"
//#include "Components/Movement.h"
#include "Components/ShootingController.h"
#include "Entity.h"
#include "Events/EventArgs.h"
#include "Events/TriggerEventArgs.h"
#include "Events/TriggerEventIDs.h"
#include "IEvent.h"
#include "Stream.h"
#include "Systems/Logging/ILoggingSystem.h"
using namespace RassEngine;
using namespace RassEngine::Components;
using namespace RassEngine::Events;
using namespace RassEngine::Systems;
namespace RassGame::Components {

WeaponUnlocker::WeaponUnlocker(): Cloneable<Component, WeaponUnlocker>{}
	, onTriggerEnter{this, &WeaponUnlocker::OnTriggerEnter}
  , onTriggerExit{this, &WeaponUnlocker::OnTriggerExit}
{}

WeaponUnlocker::WeaponUnlocker(const WeaponUnlocker &other)
	: Cloneable<Component, WeaponUnlocker>{other}
	, onTriggerEnter{this, &WeaponUnlocker::OnTriggerEnter}
	, onTriggerExit{this, &WeaponUnlocker::OnTriggerExit}
	, weaponId{other.weaponId}
	, autoSwitch{other.autoSwitch}
{}

WeaponUnlocker::~WeaponUnlocker() {
	if(Parent() == nullptr) {
		return;
	}
	Parent()->UnbindEvent(Events::TriggerEventID::OnTriggerEnter, &onTriggerEnter);
	Parent()->UnbindEvent(Events::TriggerEventID::OnTriggerExit, &onTriggerExit);
}

bool WeaponUnlocker::Initialize() {
	if(Parent() == nullptr) {
		LOG_ERROR("Cannot initialize {}: parent entity not found", NameClass());
		return false;
	}
	Parent()->BindEvent(Events::TriggerEventID::OnTriggerEnter, &onTriggerEnter);
	Parent()->BindEvent(Events::TriggerEventID::OnTriggerExit, &onTriggerExit);
	return true;
}

const std::string_view &WeaponUnlocker::NameClass() const {
	static constexpr std::string_view className = NAMEOF(WeaponUnlocker);
	return className;
}

bool WeaponUnlocker::Read(Stream &stream) {
	if(!Component::Read(stream)) {
		return false;
	}
	stream.Read("WeaponID", weaponId);
	stream.Read("AutoSwitch", autoSwitch);
	//stream.Read("DeathValue", DeathValue);
	return true;
}

bool WeaponUnlocker::OnTriggerEnter(const IEvent<Events::EventArgs> *, const Events::EventArgs &args) {
	Entity *other = dynamic_cast<const Events::TriggerEventArgs &>(args).otherEntity;
	if(!other) {
		return true;
	}
	if(auto *ctrl = other->Get<ShootingController>()) {
		ctrl->UnlockWeapon(weaponId, autoSwitch);

		Parent()->Destroy();
	}
	return true;

}
bool WeaponUnlocker::OnTriggerExit(const IEvent<Events::EventArgs> *, const Events::EventArgs &args) {
	return true;
}
}
