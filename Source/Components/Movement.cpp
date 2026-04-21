// File Name:    Movement.cpp
// Author(s):    main Niko Bekris, secondary Boyuan Hu, Steven Yacoub, Taro Omiya, Eric Fleegal
// Course:       GAM541
// Project:      RASS
// Purpose:      Component handling entity movement and velocity.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "Movement.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Component.h"
#include "Components/PhysicsBody.h"
#include "Components/Transform.h"
#include "Components/ShootingController.h"
#include "Components/TileMap.h"
#include "Entity.h"
#include "Events/Global.h"
#include "Events/GlobalEventArgs.h"
#include "IEvent.h"
#include "Events/AudioEvents.h"
#include "Systems/GlobalEvents/IGlobalEventsSystem.h"
#include "Systems/Time/ITimeSystem.h"
#include "Systems/Scene/ISceneSystem.h"
#include "Systems/Camera/ICameraSystem.h"
#include "Stream.h"
#include "TileMapData.h"
#include "Components/AnimationController.h"
#include "TileSet.h"
namespace RassEngine::Components {

const float GROUNDHEIGHT = 0.0f;

Movement::Movement() : Cloneable<Component, Movement>{}
, onFixedUpdateListener{this, &Movement::FixedUpdate}
, onUpdateListener{this, &Movement::Update}
, jumpVelocity{0.0}
, detectGround{true}
, moveSpeed{0.0} {}


Movement::Movement(const Movement &other)
	: Cloneable<Component, Movement>{other}
	, onFixedUpdateListener{this, &Movement::FixedUpdate}
	, onUpdateListener{this, &Movement::Update}
	, jumpVelocity{other.jumpVelocity}
	, jumpAudioFile{other.jumpAudioFile}
	, landAudioFile{other.landAudioFile}
	, footstepAudioFile{other.footstepAudioFile}
	, footstepInterval{other.footstepInterval}
	, detectGround{other.detectGround}
	, moveSpeed{other.moveSpeed}
	, fakeGroundJump{other.fakeGroundJump}
	, fakeGroundThreshold{other.fakeGroundThreshold}
{}

Movement::~Movement() {
	if(Systems::IGlobalEventsSystem::Get() == nullptr) {
		return;
	}
	Systems::IGlobalEventsSystem::Get()->unbind(Events::Global::FixedUpdate, &onFixedUpdateListener);
	Systems::IGlobalEventsSystem::Get()->unbind(Events::Global::Update, &onUpdateListener);
}

bool Movement::Initialize() {
	inputSystem = RassEngine::Systems::IInputSystem::Get();
	cameraSystem = RassEngine::Systems::ICameraSystem::Get();
	transform = Parent()->Get<Components::Transform>();
	physics = Parent()->Get<Components::PhysicsBody>();
	audioSource = Parent()->Get<Components::AudioSource>();

	RassEngine::Events::AudioEvents::LoadSound(jumpAudioFile);
	RassEngine::Events::AudioEvents::LoadSound(landAudioFile);
	RassEngine::Events::AudioEvents::LoadSound(footstepAudioFile);

	if(physics) {
		//physics->isKinematic = true;
	}

	if(Systems::IGlobalEventsSystem::Get() == nullptr) {
		return false;
	}

	Systems::IGlobalEventsSystem::Get()->bind(Events::Global::FixedUpdate, &onFixedUpdateListener);
	Systems::IGlobalEventsSystem::Get()->bind(Events::Global::Update, &onUpdateListener);
	return true;
}

const std::string_view &Movement::NameClass() const {
	static constexpr std::string_view className = NAMEOF(RassEngine::Components::Movement);
	return className;
}

bool Movement::Read(Stream &stream) {
	if(!Component::Read(stream)) {
		return false;
	}
	stream.Read("JumpAudioFile", jumpAudioFile);
	stream.Read("JumpVelocity", jumpVelocity);
	stream.Read("DetectGround", detectGround);
	stream.Read("MoveSpeed", moveSpeed);
	stream.Read("FakeGroundJump", fakeGroundJump);
	stream.Read("FakeGroundThreshold", fakeGroundThreshold);
	stream.Read("LandAudioFile", landAudioFile);
	stream.Read("FootstepAudioFile", footstepAudioFile);
	stream.Read("FootstepInterval", footstepInterval);
	return true;
}

// Returns true when the character can be treated as grounded:
//   - tilemap collision has set isGrounded, OR
//   - fakeGroundJump is enabled and vertical velocity is near zero
//     (covers non-tilemap surfaces that don't write isGrounded).
bool Movement::IsEffectivelyGrounded() const {
	if(physics->isGrounded) {
		return true;
	}
	if(fakeGroundJump && std::abs(physics->GetVelocity().y) <= fakeGroundThreshold) {
		return true;
	}
	return false;
}

void Movement::ChangeState(MovementState newState) {
	if(currState == newState) {
		return;
	}
	// Detect landing: Falling -> Idle or Walking
	if(currState == MovementState::Falling &&
		(newState == MovementState::Idle || newState == MovementState::Walking)) {
			rocketJumping = false;
			RassEngine::Events::AudioEvents::PlaySFXRandomPitch(landAudioFile);
			if(auto *sc = Parent()->Get<ShootingController>()) {
				sc->ForceCooldown();
			}
	}

	if(currState == MovementState::Backdash) {
		physics->SetVelocity(glm::vec3(0.0f, physics->GetVelocity().y, 0.0f));
	}

	currState = newState;

	if(currState == MovementState::Backdash) {
		backdashTimer = backdashDuration;
		backdashCooldownTimer = backdashCooldown;
	} else if(currState == MovementState::Jumping) {
		if(!rocketJumping) {
			RassEngine::Events::AudioEvents::PlaySFX(jumpAudioFile, 2.0);
		}
	}

	AnimationController* anims = Parent()->Get<AnimationController>();
	if(anims) {
		switch(currState) {
			case MovementState::Idle:{
				anims->PlayAnim("Idle");
				break;
			}
			case MovementState::Walking:
			{
				anims->PlayAnim("Walking");
				break;
			}
			case MovementState::Jumping:
			{
				anims->PlayAnim("Jumping");
				break;
			}
			case MovementState::Falling:
			{
				anims->PlayAnim("Falling");
				break;
			}
			case MovementState::Backdash:
			{
				anims->PlayAnim("Idle");
				break;
			}
		}
	}
}

// Decay impulseVelocity.x and clear it on wall contact.
// Called once per fixed update before any state logic runs.
void Movement::ApplyImpulseDecay(float dt) {
	const float decayRate = physics->isGrounded
		? physics->GetDrag() * 10.0f
		: physics->GetDrag();
	physics->impulseVelocity.x *= std::max(0.0f, 1.0f - decayRate * dt);

	if(physics->isTouchingRightWall && physics->impulseVelocity.x > 0.0f) {
		physics->impulseVelocity.x = 0.0f;
	}
	if(physics->isTouchingLeftWall && physics->impulseVelocity.x < 0.0f) {
		physics->impulseVelocity.x = 0.0f;
	}
}

bool Movement::FixedUpdate(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &) {
	float dt = Systems::ITimeSystem::Get()->GetFixedDeltaTimeSec();

	if(backdashCooldownTimer > 0.0f) {
		backdashCooldownTimer -= dt;
	}

	if(jumpBufferTimer > 0.0f) {
		jumpBufferTimer -= dt;
	}

	// Decay and wall-clear impulse once per frame before state logic
	ApplyImpulseDecay(dt);

	switch(currState) {
	case(MovementState::Idle):
		UpdateIdle(dt);
		break;
	case(MovementState::Walking):
		UpdateWalking(dt);
		break;
	case(MovementState::Jumping):
		UpdateJumping(dt);
		break;
	case(MovementState::Falling):
		UpdateFalling(dt);
		break;
	case(MovementState::Backdash):
		UpdateBackdash(dt);
		break;
	}

	// Footstep audio
	if(currState == MovementState::Walking && IsEffectivelyGrounded() &&
	   std::abs(physics->GetVelocity().x) > 0.1f) {
		footstepTimer -= dt;
		if(footstepTimer <= 0.0f) {
			RassEngine::Events::AudioEvents::PlaySFXRandomPitch(footstepAudioFile);
			footstepTimer = footstepInterval;
		}
	} else {
		footstepTimer = 0.0f;
	}

	if(cameraSystem) {
		const bool isMovingHorizontally =
			currState == MovementState::Walking ||
			currState == MovementState::Jumping ||
			currState == MovementState::Falling;

		if(isMovingHorizontally && std::abs(physics->GetVelocity().x) > 0.1f) {
			// direction: 1 = right, -1 = left
			// Negate direction so window shifts ahead of the character
			cameraSystem->SetCameraWindowOffset(
				glm::vec3(-direction * 1.0f, 0.0f, 0.0f));
		}
		//else {
		//	cameraSystem->SetCameraWindowOffset(glm::vec3(0.0f));
		//}
	}

	return true;
}

bool Movement::Update(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &) {
	if(inputSystem->IsKeyPressed(GLFW_KEY_X) ||
		inputSystem->IsKeyPressed(GLFW_KEY_UP) ||
		inputSystem->IsKeyPressed(GLFW_KEY_W) ||
		inputSystem->IsKeyPressed(GLFW_KEY_SPACE)) {
		jumpBufferTimer = jumpBufferDuration;
	}

	return true;
}

void Movement::UpdateIdle(float dt) {
	glm::vec3 velocity = physics->GetVelocity();
	// Impulse (recoil) still applies in idle
	velocity.x = physics->impulseVelocity.x;
	velocity.y += gravityFactor * dt;

	if(inputSystem->IsKeyDown(GLFW_KEY_LEFT) || inputSystem->IsKeyDown(GLFW_KEY_A) || inputSystem->IsKeyDown(GLFW_KEY_RIGHT) || inputSystem->IsKeyDown(GLFW_KEY_D)) {
		ChangeState(MovementState::Walking);
	}

	if(!isGrabbing && jumpBufferTimer > 0.0f && IsEffectivelyGrounded()) {
		jumpBufferTimer = 0.0f;
		rocketJumping = false;
		velocity.y = jumpVelocity;
		ChangeState(MovementState::Jumping);
	}

	if(!isGrabbing && inputSystem->IsKeyPressed(GLFW_KEY_Z) && IsEffectivelyGrounded()) {
		rocketJumping = false;
		if(backdashCooldownTimer <= 0.0f) {
			ChangeState(MovementState::Backdash);
		}
	}

	if(!IsEffectivelyGrounded()) {
		ChangeState(MovementState::Falling);
	}

	physics->SetVelocity(velocity);
}

void Movement::UpdateWalking(float dt) {
	glm::vec3 velocity = physics->GetVelocity();

	velocity.y += gravityFactor * dt;

	if(inputSystem->IsKeyDown(GLFW_KEY_RIGHT) || inputSystem->IsKeyDown(GLFW_KEY_D)) {
		velocity.x = moveSpeed * speedMultiplierOverride + physics->impulseVelocity.x;
		if(direction != 1) {
			direction = 1;
			glm::vec3 scale = transform->GetLocalScale();
			scale.x = std::abs(scale.x);
			transform->SetLocalScale(scale);
		}
	} else if(inputSystem->IsKeyDown(GLFW_KEY_LEFT)
		|| inputSystem->IsKeyDown(GLFW_KEY_A)
		|| inputSystem->IsKeyDown(GLFW_KEY_D)) {
		velocity.x = -moveSpeed * speedMultiplierOverride + physics->impulseVelocity.x;
		if(direction != -1) {
			direction = -1;
			glm::vec3 scale = transform->GetLocalScale();
			scale.x = -std::abs(scale.x);
			transform->SetLocalScale(scale);
		}
	} else {
		ChangeState(MovementState::Idle);
		velocity.x = physics->impulseVelocity.x;
	}

	if(!isGrabbing && jumpBufferTimer > 0.0f && IsEffectivelyGrounded()) {
		jumpBufferTimer = 0.0f;
		rocketJumping = false;
		velocity.y = jumpVelocity;
		ChangeState(MovementState::Jumping);
	} else if(!IsEffectivelyGrounded()) {
		coyoteTimer = coyoteTimeDuration;
		ChangeState(MovementState::Falling);
	}

	if(!isGrabbing && (inputSystem->IsKeyPressed(GLFW_KEY_Z) || inputSystem->IsKeyPressed(GLFW_KEY_Q)) && IsEffectivelyGrounded()) {
		rocketJumping = false;
		if(backdashCooldownTimer <= 0.0f) {
			ChangeState(MovementState::Backdash);
		}
	}

	physics->SetVelocity(velocity);
}

void Movement::UpdateJumping(float dt) {
	glm::vec3 velocity = physics->GetVelocity();

	velocity.y += gravityFactor * dt;

	if(inputSystem->IsKeyDown(GLFW_KEY_RIGHT) || inputSystem->IsKeyDown(GLFW_KEY_D)) {
		velocity.x = moveSpeed + physics->impulseVelocity.x;
		direction = 1;
	} else if(inputSystem->IsKeyDown(GLFW_KEY_LEFT) || inputSystem->IsKeyDown(GLFW_KEY_A)) {
		velocity.x = -moveSpeed + physics->impulseVelocity.x;
		direction = -1;
	} else {
		velocity.x = physics->impulseVelocity.x;
	}

	if(!rocketJumping &&
		!(inputSystem->IsKeyDown(GLFW_KEY_X)
		|| inputSystem->IsKeyDown(GLFW_KEY_UP)
		|| inputSystem->IsKeyDown(GLFW_KEY_W)
		|| inputSystem->IsKeyDown(GLFW_KEY_SPACE)) && velocity.y > 0) {
			velocity.y *= 0.5f;
	}

	if(velocity.y <= 0) {
		ChangeState(MovementState::Falling);
	}

	physics->SetVelocity(velocity);
}

void Movement::UpdateFalling(float dt) {
	//DetectGround();
	glm::vec3 velocity = physics->GetVelocity();

	velocity.y += gravityFactor * dt;

	if(coyoteTimer > 0.0f) {
		coyoteTimer -= dt;
	}

	if(inputSystem->IsKeyDown(GLFW_KEY_RIGHT) || inputSystem->IsKeyDown(GLFW_KEY_D)) {
		velocity.x = moveSpeed + physics->impulseVelocity.x;
	} else if(inputSystem->IsKeyDown(GLFW_KEY_LEFT) || inputSystem->IsKeyDown(GLFW_KEY_A)) {
		velocity.x = -moveSpeed + physics->impulseVelocity.x;
	} else {
		velocity.x = physics->impulseVelocity.x;
	}

	if(!isGrabbing && jumpBufferTimer > 0.0f && (IsEffectivelyGrounded() || coyoteTimer > 0.0f)) {
		jumpBufferTimer = 0.0f;
		coyoteTimer = 0.0f;
		rocketJumping = false;
		velocity.y = jumpVelocity;
		physics->SetVelocity(velocity);
		ChangeState(MovementState::Jumping);
		return;
	}

	if(IsEffectivelyGrounded()) {
		if(std::abs(velocity.x) > 0.1f) {
			ChangeState(MovementState::Walking);
		} else {
			ChangeState(MovementState::Idle);
		}
	}

	physics->SetVelocity(velocity);
}

void Movement::UpdateBackdash(float dt) {
	backdashTimer -= dt;
	glm::vec3 velocity = physics->GetVelocity();

	velocity.y += gravityFactor * dt;

	float slideFactor = std::max(0.0f, backdashTimer / backdashDuration);

	velocity.x = -direction * backdashSpeed * slideFactor + physics->impulseVelocity.x;

	physics->SetVelocity(velocity);

	if(backdashTimer <= 0) {
		ChangeState(MovementState::Idle);
	}
}

void Movement::MoveSprite(float dt) {
	glm::vec3 velocity = physics->GetVelocity();

	if(transform && physics) {
		// Always decay impulse regardless of input state
		const float decayRate = physics->isGrounded
			? physics->GetDrag() * 10.0f
			: physics->GetDrag();
		physics->impulseVelocity.x *= std::max(0.0f, 1.0f - decayRate * dt);

		// Always clear impulse against tile walls
		if(physics->isTouchingRightWall && physics->impulseVelocity.x > 0.0f) {
			physics->impulseVelocity.x = 0.0f;
		}
		if(physics->isTouchingLeftWall && physics->impulseVelocity.x < 0.0f) {
			physics->impulseVelocity.x = 0.0f;
		}

		// Impulse is additive with input: shooting while moving gives momentum interaction
		if(inputSystem->IsKeyDown(GLFW_KEY_RIGHT) || inputSystem->IsKeyDown(GLFW_KEY_D)) {
			velocity.x = moveSpeed * dt + physics->impulseVelocity.x;

		} else if(inputSystem->IsKeyDown(GLFW_KEY_LEFT) || inputSystem->IsKeyDown(GLFW_KEY_A)) {
			velocity.x = -moveSpeed * dt + physics->impulseVelocity.x;

		} else {
			velocity.x = physics->impulseVelocity.x;
		}

		physics->SetVelocity(velocity);
	}
}

void Movement::Jump() {
	glm::vec3 velocity = physics->GetVelocity();

	if(transform && physics) {
		if(inputSystem->IsKeyDown(GLFW_KEY_UP)
			|| inputSystem->IsKeyDown(GLFW_KEY_W)
			|| inputSystem->IsKeyDown(GLFW_KEY_SPACE)){
			velocity.y = jumpVelocity;

			//Need to split this for RocketSound and JumpSound
			if(audioSource) {
				audioSource->Play();
			}
		}
		physics->SetAcceleration(glm::vec3(0.0f, -9.8f, 0.0f));
		physics->SetVelocity(velocity);
	}
}

void Movement::DetectGround() {
	/*
	glm::vec3 velocity = physics->GetVelocity();

	if(transform && physics) {
		// Check if we are at or below ground
		if(transform->GetPosition().y < GROUNDHEIGHT) {

			//// Check if we are moving downwards
			//if(velocity.y <= 0.0f) {
			//	velocity.y = 0.0f;

			//	glm::vec3 position = transform->GetPosition();
			//	position.y = GROUNDHEIGHT;
			//	transform->SetPosition(position);
			//}
//RassEngine::Systems::ISceneSystem::Get()->Restart();
			if(!HasCheckpoint()) {
				LOG_WARNING("{}: Attempted to respawn at checkpoint, but no checkpoint set", NameClass());
				RassEngine::Systems::ISceneSystem::Get()->Restart();
				return;
			}
			RespawnAtCheckpoint();
		}
		physics->SetVelocity(velocity);
	}*/
}
void Movement::SetLastCheckpoint(Entity *cpEntity, unsigned row, unsigned col) {
	cpLayerEntity = cpEntity;
	cpRow = row;
	cpCol = col;
	LOG_INFO("{}: Checkpoint set to tile ({}, {})", NameClass(), row, col);
}

void Movement::RespawnAtCheckpoint() {
	if(!transform || !physics) {
		LOG_WARNING("{}: Cannot respawn — missing components", NameClass());
		return;
	}
	if(!HasCheckpoint()) {

		LOG_WARNING("{}: Attempted to respawn at checkpoint, but no checkpoint set", NameClass());
		RassEngine::Systems::ISceneSystem::Get()->Restart();
		return;
	}

	auto *cpTileMap = cpLayerEntity->Get<Components::TileMap>();
	if(!cpTileMap) {
		LOG_WARNING("{}: CP layer entity has no TileMap component", NameClass());
		return;
	}

	const TileMapData *data = cpTileMap->GetTileMapData();
	if(!data) {
		LOG_WARNING("{}: CP TileMap has no TileMapData", NameClass());
		return;
	}

	glm::vec3 mapWorldPos(0.0f);
	glm::vec3 mapScale(1.0f);
	if(auto *cpTransform = cpLayerEntity->Get<Components::Transform>()) {
		mapWorldPos = cpTransform->GetPosition();
		mapScale = cpTransform->GetLocalScale();
	}

	float tileLocalX, tileLocalY;
	data->TileToWorld(cpRow, cpCol, tileLocalX, tileLocalY);

	const float spawnWorldX = (tileLocalX+0.5) * mapScale.x + mapWorldPos.x;
	const float spawnWorldY = (tileLocalY+0.7) * mapScale.y + mapWorldPos.y;

	transform->SetPosition(glm::vec3(spawnWorldX, spawnWorldY, 0.0f));
	physics->SetVelocity(glm::vec3(0.0f));
	LOG_INFO("{}: Respawned at tile ({}, {}) → world ({:.2f}, {:.2f})",
		NameClass(), cpRow, cpCol, spawnWorldX, spawnWorldY);
}

}
