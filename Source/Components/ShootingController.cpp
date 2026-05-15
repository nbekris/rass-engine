// File Name:    ShootingController.cpp
// Author(s):    main Boyuan Hu, secondary Niko Bekris, Steven Yacoub
// Course:       GAM541
// Project:      RASS
// Purpose:      Component handling player shooting mechanics.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "ShootingController.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Component.h"
#include "Components/PhysicsBody.h"
#include "Components/Sprite.h"
#include "Components/Transform.h"
#include "Components/Movement.h"
#include "Components/Behaviors/BulletBehavior.h"
#include "Entity.h"
#include "Systems/Entity/IEntityFactory.h"
#include "Events/Global.h"
#include "Events/GlobalEventArgs.h"
#include "IEvent.h"
#include "Events/AudioEvents.h"
#include "Systems/GlobalEvents/IGlobalEventsSystem.h"
#include "Systems/Logging/ILoggingSystem.h"
#include "Systems/PauseMenu/IPauseMenuSystem.h"

#include "Stream.h"
using namespace RassEngine::Systems;
namespace RassEngine::Components {

ShootingController::ShootingController()
	: Cloneable<Component, ShootingController>{}
	, onUpdateListener{this, &ShootingController::Update} {}

ShootingController::ShootingController(const ShootingController &other)
	: Cloneable<Component, ShootingController>{other}
	, onUpdateListener{this, &ShootingController::Update}
	, crosshairName{other.crosshairName}
	, armName{other.armName}
	, weapons_{other.weapons_}
	, activeWeaponIndex_{other.activeWeaponIndex_}
{}

ShootingController::~ShootingController() {
	if(Systems::IGlobalEventsSystem::Get() == nullptr) {
		return;
	}
	Systems::IGlobalEventsSystem::Get()->unbind(Events::Global::Update, &onUpdateListener);
}

bool ShootingController::Initialize() {
	transform = Parent()->Get<Components::Transform>();
	physics = Parent()->Get<Components::PhysicsBody>();
	audioSource = Parent()->Get<Components::AudioSource>();

	if(Systems::IGlobalEventsSystem::Get() == nullptr) {
		return false;
	}

	Systems::IGlobalEventsSystem::Get()->bind(Events::Global::Update, &onUpdateListener);

	if(!entitiesLinked_) {
		LinkEntities();
	}
	return true;
}

void ShootingController::LinkEntities() {
	if(ISceneSystem::Get() == nullptr) {
		return;
	}
	if(!crosshairName.empty() && !crosshair) {
		crosshair = ISceneSystem::Get()->FindEntity(crosshairName);
		if(!crosshair) {
			LOG_WARNING("{}: Crosshair entity \"{}\" not found", NameClass(), crosshairName);
		}
	}

	if(!armName.empty() && !arm) {
		arm = ISceneSystem::Get()->FindEntity(armName);
		if(arm) {
			arm->GetTransform()->SetParentTransform(transform, false);
		} else {
			LOG_WARNING("{}: Arm entity \"{}\" not found", NameClass(), armName);
		}
	}
	// Cache active camera and record its base offset from the data file
	if(!camera_) {
		if(auto *cameraSystem = ICameraSystem::Get()) {
			camera_ = cameraSystem->GetActiveCamera();
		}
	}
	SwitchWeapon(activeWeaponIndex_);
	entitiesLinked_ = true;
}

const std::string_view &ShootingController::NameClass() const {
	static constexpr std::string_view className = NAMEOF(RassEngine::Components::ShootingController);
	return className;
}

bool ShootingController::Read(Stream &stream) {
	if(!Component::Read(stream)) {
		return false;
	}
	stream.Read("CrosshairName", crosshairName);
	stream.Read("ArmName", armName);
	stream.Read("ActiveWeaponIndex", activeWeaponIndex_);
	weapons_.clear();
	stream.ReadArray("Weapons", [&] () {
		WeaponProfile profile;
		stream.Read("BulletArchetype", profile.bulletArchetype);
		stream.Read("GunTexturePath", profile.gunTexturePath);
		stream.Read("RecoilStrength", profile.recoilStrength);
		stream.Read("FireRate", profile.fireRate);
		stream.Read("BulletsPerShot", profile.bulletsPerShot);
		stream.Read("SpreadAngle", profile.spreadAngle);
		stream.Read("FireCooldownTime", profile.fireCooldownTime);

		stream.Read("FireAudioFile", profile.fireAudioFile);
		RassEngine::Events::AudioEvents::LoadSound(profile.fireAudioFile);
		stream.Read("Locked", profile.locked);
		// Per-weapon camera behavior
		std::string cameraMode;
		stream.Read("CameraMode", cameraMode);
		if(cameraMode == "Tight") {
			profile.cameraMode = WeaponProfile::CameraMode::Tight;
		}
		else if(cameraMode == "AimLookAhead") {
			profile.cameraMode = WeaponProfile::CameraMode::AimLookAhead;
		}
		stream.Read("AimDriftStrength", profile.aimDriftStrength);
		stream.Read("AimExploreRadius", profile.aimExploreRadius);
		stream.Read("AimExploreZoomOut", profile.aimExploreZoomOut);
		weapons_.push_back(std::move(profile));
		});
	return true;
}

void ShootingController::StartGrapple() {

}
bool ShootingController::UnlockWeapon(int weaponId, bool autoSwitch) {
	if(weaponId < 0 || weaponId >= (weapons_.size())) {
		return false;
	}
	weapons_[weaponId].locked = false;
	if(autoSwitch) {
		SwitchWeapon(weaponId);
	}
	return true;
}
bool ShootingController::Update(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &) {
	// Freeze aiming, crosshair, and shooting while paused
	if(Systems::IPauseMenuSystem::Get() && Systems::IPauseMenuSystem::Get()->IsPaused()) {
		return true;
	}

	//delay linking entities until the first update, to ensure all entities are loaded


	calculateAimDirection();
	UpdateCrosshairPosition();
	UpdateArmRotation();
	UpdateCameraOffset();
	timmer += Systems::ITimeSystem::Get()->GetDeltaTimeSec();

	// Sample triggers once per frame for threshold-crossing detection
	const float rightTrigger = IInputSystem::Get() ? IInputSystem::Get()->GetGamepadAxis(GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER) : -1.0f;
	const float leftTrigger  = IInputSystem::Get() ? IInputSystem::Get()->GetGamepadAxis(GLFW_GAMEPAD_AXIS_LEFT_TRIGGER)  : -1.0f;
	const bool rightTriggerJustPressed = (prevRightTrigger_ <= TRIGGER_THRESHOLD) && (rightTrigger > TRIGGER_THRESHOLD);
	const bool leftTriggerJustPressed  = (prevLeftTrigger_  <= TRIGGER_THRESHOLD) && (leftTrigger  > TRIGGER_THRESHOLD);

	// God mode: bypass per-weapon cooldown when rapidFire_ is active
	if(activeWeaponIndex_ >= 0 && activeWeaponIndex_ < static_cast<int>(weapons_.size())) {
		const float effectiveCooldown = rapidFire_ ? 0.0f : weapons_[activeWeaponIndex_].fireCooldownTime;
		if(timmer > effectiveCooldown) {
			const bool fireInput = (IInputSystem::Get() && IInputSystem::Get()->IsMouseButtonClicked(GLFW_MOUSE_BUTTON_LEFT))
				|| rightTriggerJustPressed;
			if(fireInput) {
				ApplyRecoil();
				SpawnBullet();
				timmer = 0.0f;
			}
		}
	}

	const bool switchInput = (IInputSystem::Get() && IInputSystem::Get()->IsMouseButtonClicked(GLFW_MOUSE_BUTTON_RIGHT))
		|| leftTriggerJustPressed
		|| (IInputSystem::Get() && IInputSystem::Get()->IsGamepadButtonPressed(GLFW_GAMEPAD_BUTTON_X));

	if(switchInput && (weapons_.size() > 0)) {
		activeWeaponIndex_++;
		if(activeWeaponIndex_ >= static_cast<int>(weapons_.size())) {
			activeWeaponIndex_ = 0;
		}
		int tempcounter = 0;
		while(activeWeaponIndex_ >= 0 && weapons_[activeWeaponIndex_].locked) {
			activeWeaponIndex_++;
			tempcounter++;
			if(activeWeaponIndex_ >= static_cast<int>(weapons_.size())) {
				activeWeaponIndex_ = 0;
			}
			if(tempcounter >= static_cast<int>(weapons_.size())) {
				activeWeaponIndex_ = -1;
				break;
			}
		}
		SwitchWeapon(activeWeaponIndex_);
	}

	if(switchInput && (weapons_.size() == 0)) {
		StartGrapple();
	}

	prevRightTrigger_ = rightTrigger;
	prevLeftTrigger_  = leftTrigger;
	return true;
}
void ShootingController::SwitchWeapon(int index) {
	if(index < 0 || index >= (weapons_.size())) {
		if(index == -1) {
			arm->Get<Components::Sprite>()->SetTexture("Assets/Textures/transparent.png");
		}

		//LOG_WARNING("{}: Invalid weapon index {}", NameClass(), index);
		return;
	}
	if(weapons_[index].locked) {
		return; // Cannot switch away from current weapon if it's locked
	}
	activeWeaponIndex_ = index;

	// Optionally update arm sprite here using weapons_[activeWeaponIndex_].gunTexturePath
	arm->Get<Components::Sprite>()->SetTexture(weapons_[activeWeaponIndex_].gunTexturePath);
}
void ShootingController::ApplyRecoil() {
	if(!physics || glm::length(aimDirection) < std::numeric_limits<float>::epsilon()) return;
	if(activeWeaponIndex_ <= 0) return;
	if(weapons_[activeWeaponIndex_].locked) return;
	// Recoil is opposite to aim direction
	glm::vec3 recoilDir = glm::vec3(-aimDirection.x, -aimDirection.y, 0.0f);

	glm::vec3 xImpulse = glm::vec3(recoilDir.x * weapons_[activeWeaponIndex_].recoilStrength, 0.0f, 0.0f);
	physics->AddImpulse(xImpulse);

	glm::vec3 velocity = physics->GetVelocity();
	float yRecoil = recoilDir.y * weapons_[activeWeaponIndex_].recoilStrength * physics->GetInverseMass();

	if(yRecoil > 0.0f) {
		if(velocity.y < yRecoil) {
			velocity.y = yRecoil;
		}

		if(Movement *movement = Parent()->Get<Components::Movement>()) {
			movement->rocketJumping = true;
			movement->ChangeState(Components::Movement::MovementState::Jumping);
		}
	} else {
		velocity.y += yRecoil;
	}

	physics->SetVelocity(velocity);
}

void ShootingController::calculateAimDirection() {
	if(!IInputSystem::Get() || !ICameraSystem::Get() || !transform) return;

	// Right stick aim: read axes, apply deadzone, flip Y (GLFW Y+ = down, world Y+ = up)
	const float stickX = IInputSystem::Get()->GetGamepadAxis(GLFW_GAMEPAD_AXIS_RIGHT_X);
	const float stickY = -IInputSystem::Get()->GetGamepadAxis(GLFW_GAMEPAD_AXIS_RIGHT_Y);
	const glm::vec2 stick{stickX, stickY};

	const glm::vec2 aimOrigin = transform->GetPosition2D() + glm::vec2(0.0f, AIM_HEIGHT_OFFSET);

	if(glm::length(stick) > AIM_STICK_DEADZONE) {
		usingGamepadAim_ = true;
		aimDirection = glm::normalize(stick);
		aimWorldPos_ = aimOrigin + aimDirection * CROSSHAIR_DISTANCE;
	} else if(IInputSystem::Get()->IsGamepadConnected()) {
		// Stick is neutral — rest crosshair directly in front of the player at body height
		usingGamepadAim_ = true;
		float facingX = (transform->GetLocalScale().x >= 0.0f) ? 1.0f : -1.0f;
		aimDirection = glm::vec2(facingX, 0.0f);
		aimWorldPos_ = aimOrigin + aimDirection * CROSSHAIR_DISTANCE;
	} else {
		usingGamepadAim_ = false;
		glm::vec2 mouseNDC = IInputSystem::Get()->GetMousePositionViewport();
		aimWorldPos_ = ICameraSystem::Get()->ViewportToWorld(mouseNDC);
		glm::vec2 toMouse = aimWorldPos_ - transform->GetPosition2D();
		if(glm::length(toMouse) > std::numeric_limits<float>::epsilon()) {
			aimDirection = glm::normalize(toMouse);
		} else {
			aimDirection = glm::vec2(0.0f);
		}
	}

	// Flip character to face the aim direction
	if(transform && std::abs(aimDirection.x) > std::numeric_limits<float>::epsilon()) {
		glm::vec3 scale = transform->GetLocalScale();
		float absX = std::abs(scale.x);
		scale.x = (aimDirection.x >= 0.0f) ? absX : -absX;
		transform->SetLocalScale(scale);
	}
}
void ShootingController::UpdateCameraOffset() {
	if(!camera_) {
		if(auto *cameraSystem = ICameraSystem::Get()) {
			camera_ = cameraSystem->GetActiveCamera();
		}
	}
	if(!camera_ || weapons_.empty()) return;
	if(activeWeaponIndex_ < 0) return;
	const WeaponProfile &wp = weapons_[activeWeaponIndex_];

	if(wp.cameraMode == WeaponProfile::CameraMode::Tight) {
		camera_->targetAimOffset = glm::vec3(0.0f);
		//camera_->targetZoom = camera_->basicZoom;
		return;
	}
	auto *cameraSystem = ICameraSystem::Get();
	if(!cameraSystem) return;

	const glm::vec2 screenCenterWorld = cameraSystem->ViewportToWorld(glm::vec2(0.0f, 0.0f));
	const glm::vec2 toAim = aimWorldPos_ - screenCenterWorld;
	const float aimDist = glm::length(toAim);

	if(aimDist < std::numeric_limits<float>::epsilon()) return;

	const glm::vec2 aimDirFromCenter = toAim / aimDist;

	// Dead zone: t = 0 when cursor is within aimExploreRadius,
	// ramps smoothly to 1 at 2× aimExploreRadius.
	// If aimExploreRadius == 0, no dead zone (full effect immediately).
	float t = 1.0f;
	if(wp.aimExploreRadius > 0.0f) {
		t = glm::clamp((aimDist - wp.aimExploreRadius) / wp.aimExploreRadius, 0.0f, 1.0f);
	}

	const glm::vec3 zoomRatio = camera_->zoom / camera_->basicZoom;
	const glm::vec3 safeZoomRatio = glm::max(zoomRatio, glm::vec3(std::numeric_limits<float>::epsilon()));
	camera_->targetAimOffset = glm::vec3(aimDirFromCenter * (wp.aimDriftStrength * t), 0.0f) / safeZoomRatio;
	//camera_->targetZoom = camera_->basicZoom * (1.0f - t * wp.aimExploreZoomOut);
}

void ShootingController::SpawnBullet() const {
	if(!arm || !ISceneSystem::Get() || weapons_.empty()) return;
	if(activeWeaponIndex_ < 0) return;
	const WeaponProfile &wp = weapons_[activeWeaponIndex_];
	if (wp.locked) return;
	for(int i = 0; i < wp.bulletsPerShot; ++i) {
		std::unique_ptr<Entity> bullet = IEntityFactory::Get()->Build(wp.bulletArchetype);
		if(!bullet) {
			LOG_ERROR("{}: Failed to spawn bullet", NameClass()); continue;
		}

		Components::Transform *bulletTransform = bullet->GetTransform();
		Components::Transform *armTransform = arm->GetTransform();
		if(!bulletTransform || !armTransform) continue;

		bulletTransform->SetPosition(armTransform->GetPosition());

		// calculate angle offset for spread
		float angleOffset = 0.0f;
		if(wp.bulletsPerShot > 1) {
			float step = wp.spreadAngle / static_cast<float>(wp.bulletsPerShot - 1);
			angleOffset = -wp.spreadAngle * 0.5f + step * static_cast<float>(i);
		}
		float baseAngle = std::atan2(aimDirection.y, aimDirection.x);
		float finalAngle = baseAngle + angleOffset;
		bulletTransform->SetRotationRad(finalAngle);

		Components::PhysicsBody *bp = bullet->Get<Components::PhysicsBody>();
		if(bp) {
			auto bulletBehavior = bullet->Get<Components::Behaviors::BulletBehavior>();
			float spd = bulletBehavior ? bulletBehavior->GetSpeed() : 10.0f;
			bp->SetVelocity(glm::vec3(std::cos(finalAngle) * spd,
				std::sin(finalAngle) * spd, 0.0f));
		}
		//bullet->Initialize();
		RassEngine::Events::AudioEvents::PlaySFXRandomPitch(wp.fireAudioFile);
		ISceneSystem::Get()->AddEntity(std::move(bullet));
	}
}
void ShootingController::UpdateCrosshairPosition() {
	if(!crosshair || !IInputSystem::Get() || !IRenderSystem::Get()) return;

	float aspectRatio = static_cast<float>(IRenderSystem::Get()->getScreenWidth())
		/ static_cast<float>(IRenderSystem::Get()->getScreenHeight());

	glm::vec2 ndc;
	if(usingGamepadAim_ && ICameraSystem::Get()) {
		// Place crosshair at the aim world position computed in calculateAimDirection
		ndc = ICameraSystem::Get()->WorldToScreenViewport(glm::vec3(aimWorldPos_, 0.0f));
		ndc.x = glm::clamp(ndc.x, -1.0f, 1.0f);
		ndc.y = glm::clamp(ndc.y, -1.0f, 1.0f);
	} else {
		ndc = IInputSystem::Get()->GetMousePositionViewport();
#ifndef _DEBUG
		ndc.x = glm::clamp(ndc.x, -1.0f, 1.0f);
		ndc.y = glm::clamp(ndc.y, -1.0f, 1.0f);
#endif
	}

	crosshair->GetTransform()->SetPosition(glm::vec3(
		ndc.x * aspectRatio,
		ndc.y,
		crosshair->GetTransform()->GetPosition().z
	));
}

void ShootingController::UpdateArmRotation() {
	if(!arm || glm::length(aimDirection) < std::numeric_limits<float>::epsilon()) return;
	constexpr float spriteOrientationOffset = 0.0f;
	float angle = std::atan2(aimDirection.y, aimDirection.x) + spriteOrientationOffset;
	arm->GetTransform()->SetRotationRad(transform->GetLocalScale().x < 0.0f?-angle:angle);

}
}
