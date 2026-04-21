// File Name:    ShootingController.h
// Author(s):    main Boyuan Hu, secondary Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Component handling player shooting mechanics.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <string_view>

#include "Component.h"
#include "Cloneable.h"
#include "Events/GlobalEventArgs.h"
#include "Events/GlobalEventListener.h"
#include "IEvent.h"
#include "Systems/Input/IInputSystem.h"
#include "Systems/Audio/IAudioSystem.h"
#include "Systems/Camera/ICameraSystem.h"
#include "Systems/Scene/ISceneSystem.h"
#include "Systems/Render/IRenderSystem.h"
#include "Systems/Time/ITimeSystem.h"
#include "Components/AudioSource.h"
#include "Components/Transform.h"
#include "Components/PhysicsBody.h"
#include "Components/Camera.h"

namespace RassEngine::Components {
class ShootingController : public Cloneable<Component, ShootingController> {
public:

	ShootingController();
	ShootingController(const ShootingController &other);
	virtual ~ShootingController() override;

	virtual bool Initialize() override;
	virtual const std::string_view &NameClass() const override;
	virtual bool Read(Stream &stream) override;
	glm::vec2 GetAimDirection() const {
		return aimDirection;
	}
	void ForceCooldown() {
		timmer = 100.0f;
	}
	void SetRapidFire(bool enabled) {
		rapidFire_ = enabled;
	}
	bool UnlockWeapon(int weaponId, bool autoSwitch = true);
	bool IsRapidFire() const {
		return rapidFire_;
	}
	//	void MoveSprite();
	//	void Jump();
	//	void DetectGround();
private:
	bool Update(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &);
	void LinkEntities();
	void calculateAimDirection();
	void UpdateCrosshairPosition();
	void UpdateArmRotation();
	void UpdateCameraOffset();
	void ApplyRecoil();
	void SwitchWeapon(int index);

private:
	Entity *crosshair{nullptr};
	Entity *arm{nullptr};
	Components::Camera *camera_{nullptr};
	Components::AudioSource *audioSource{nullptr};
	Components::Transform *transform{nullptr};
	Components::PhysicsBody *physics{nullptr};
	void SpawnBullet() const;
	Events::GlobalEventListener<ShootingController> onUpdateListener;

	bool isGrappling{false};
	glm::vec2 grapplePoint{0.0f, 0.0f};

	float maxGrappleDistance{15.0f};
	float grapplePull{20.0f};
	float originalGravityScale{1.0f};

	void StartGrapple();
	void Grapple();
	void EndGrapple();

	std::string crosshairName;
	std::string armName;
	glm::vec2 mousePos{0.0f, 0.0f};
	glm::vec2 aimDirection{0.0f, 0.0f};
	glm::vec2 aimWorldPos_{0.0f, 0.0f};
	glm::vec3 cameraBaseOffset_{0.0f, 0.0f, 0.0f};
	float cameraAimOffsetStrength_{0.0f};
	float timmer{0.0f};
	bool entitiesLinked_{false};

	struct WeaponProfile {
		enum class CameraMode {
			AimLookAhead,  // Camera drifts toward aim + exploration zoom out
			Tight,         // No aim camera change (rocket jump / high-recoil weapons)
		};
		std::string bulletArchetype;
		std::string gunTexturePath;
		std::string fireAudioFile;
		float       recoilStrength{10.0f};
		float       fireRate{0.2f};
		int         bulletsPerShot{1};
		float       spreadAngle{0.0f};//Radian
		float			  fireCooldownTime{0.0f};
		CameraMode  cameraMode{CameraMode::AimLookAhead};
		float       aimDriftStrength{0.0f};   // World units: how far camera drifts in aim direction
		float       aimExploreRadius{3.0f};   // World units: cursor distance before zoom-out begins
		float       aimExploreZoomOut{0.0f};  // Max zoom-out factor [0, 1), 0 = disabled
		bool        locked{false};            // If true, weapon cannot be switched away from (e.g. while grappling)
	};
	std::vector<WeaponProfile> weapons_;
	int activeWeaponIndex_{0};
	bool rapidFire_{false};
};
}

