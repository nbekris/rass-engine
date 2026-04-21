// File Name:    Movement.h
// Author(s):    main Niko Bekris, secondary Boyuan Hu, Steven Yacoub, Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Component handling entity movement and velocity.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <string_view>

#include "Component.h"
#include "Cloneable.h"
#include "Components/TileMap.h"
#include "Events/GlobalEventArgs.h"
#include "Events/GlobalEventListener.h"
#include "IEvent.h"
#include "Systems/Input/IInputSystem.h"
#include "Systems/Camera/ICameraSystem.h" 
#include "Components/AudioSource.h"
#include "Components/Transform.h"
#include "Components/PhysicsBody.h"
#include "TileMapData.h"
#include "TileSet.h"
namespace RassEngine::Components {
class Movement : public Cloneable<Component, Movement> {

public:
	enum class MovementState {
		Idle, Walking, Jumping, Falling, Backdash
	};

	Movement();
	Movement(const Movement &other);
	virtual ~Movement() override;

	virtual bool Initialize() override;
	virtual const std::string_view &NameClass() const override;
	virtual bool Read(Stream &stream) override;
	void MoveSprite(float dt);
	void Jump();
	void DetectGround();
	void ChangeState(MovementState newState);
	void SetLastCheckpoint(Entity *cpEntity, unsigned row, unsigned col);
	bool HasCheckpoint() const {
		return cpLayerEntity != nullptr;
	}
	void RespawnAtCheckpoint();
private:
	bool FixedUpdate(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &);
	bool Update(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &);

	void UpdateIdle(float dt);
	void UpdateWalking(float dt);
	void UpdateJumping(float dt);
	void UpdateFalling(float dt);
	void UpdateBackdash(float dt);
	void ApplyImpulseDecay(float dt);
	
	// Returns true if tilemap-grounded, or if fakeGroundJump is enabled
	// and vertical velocity is within fakeGroundThreshold (non-tilemap surfaces).
	bool IsEffectivelyGrounded() const;

private:
	RassEngine::Systems::IInputSystem *inputSystem{nullptr};
	RassEngine::Systems::ICameraSystem *cameraSystem{nullptr};
	Components::AudioSource *audioSource{nullptr};
	Components::Transform *transform{nullptr};
	Components::PhysicsBody *physics{nullptr};

	Events::GlobalEventListener<Movement> onFixedUpdateListener;
	Events::GlobalEventListener<Movement> onUpdateListener;

	MovementState currState = MovementState::Idle;

public:
	bool isGrabbing{false};
	bool rocketJumping{false};
	float speedMultiplierOverride{1.0f};

private:
	int direction{1};
	float backdashTimer{0.0f};
	float backdashDuration{0.35f};
	float backdashCooldownTimer{0.0f};
	float backdashCooldown{0.6f};
	float backdashSpeed{7.5f};

	float jumpBufferTimer{0.0f};
	float jumpBufferDuration{0.15f};

	float coyoteTimer{0.0f};
	float coyoteTimeDuration{0.05f};

	std::string jumpAudioFile;
	std::string landAudioFile;
	std::string footstepAudioFile;
	float footstepInterval{0.35f};
	float footstepTimer{0.0f};
	float gravityFactor{-15.0f};
	float jumpVelocity;
	bool detectGround{true};
	float moveSpeed;

	// When true, treats velocity.y ~= 0 as grounded (for non-tilemap surfaces).
	bool fakeGroundJump{true};
	// Absolute y-velocity threshold for fakeGroundJump. Tune to match gravity step.
	float fakeGroundThreshold{0.1f};
	// Last activated checkpoint position in world space (set by TileCheckPoint)
	Entity *cpLayerEntity{nullptr};
	unsigned cpRow{UINT_MAX};
	unsigned cpCol{UINT_MAX};
};
}
