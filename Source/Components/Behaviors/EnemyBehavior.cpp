// File Name:    EnemyBehavior.cpp
// Author(s):    main Boyuan Hu, secondary Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Behavior component for enemy AI logic.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "EnemyBehavior.h"

#include "BulletBehavior.h"
#include "Cloneable.h"
#include "Component.h"
#include "Components/HealthComponent.h"
#include "Entity.h"
#include "Events/EventArgs.h"
#include "Events/TriggerEventArgs.h"
#include "Events/TriggerEventIDs.h"
#include "IEvent.h"
#include "Stream.h"
#include "Components/KnockbackComponent.h"
#include "Components/Transform.h"
#include "Systems/Logging/ILoggingSystem.h"
#include "Systems/GlobalEvents/IGlobalEventsSystem.h"
#include "Events/Global.h"
// For god mode queries (2x damage + player invulnerability)
#include "Systems/Cheat/ICheatSystem.h"

namespace RassEngine::Components::Behaviors {

EnemyBehavior::EnemyBehavior()
	: Cloneable<Component, EnemyBehavior>{}
	, onTriggerEnter{this, &EnemyBehavior::OnTriggerEnter}
	, onUpdateListener{this, &EnemyBehavior::Update} {}

EnemyBehavior::EnemyBehavior(const EnemyBehavior &other)
	: Cloneable<Component, EnemyBehavior>{other}
	, onTriggerEnter{this, &EnemyBehavior::OnTriggerEnter}
	, onUpdateListener{this, &EnemyBehavior::Update}
	, contactDamage{other.contactDamage}
	, playerContactDamage{other.playerContactDamage}
	, enemyFlags{other.enemyFlags}
	, speed{other.speed}
	, targetDirection{other.targetDirection} {}

EnemyBehavior::~EnemyBehavior() {
	if(Systems::IGlobalEventsSystem::Get() != nullptr) {
		Systems::IGlobalEventsSystem::Get()->unbind(Events::Global::Update, &onUpdateListener);
	}
	if(Parent() == nullptr) {
		return;
	}
	Parent()->UnbindEvent(Events::TriggerEventID::OnTriggerEnter, &onTriggerEnter);

}

bool EnemyBehavior::Initialize() {
	if(Parent() == nullptr) {
		LOG_ERROR("Cannot initialize {}: parent entity not found", NameClass());
		return false;
	}
	if(Systems::IGlobalEventsSystem::Get() == nullptr) {
		LOG_ERROR("Cannot bind to {}", NAMEOF(Systems::IGlobalEventsSystem));
		return false;
	}
	Parent()->BindEvent(Events::TriggerEventID::OnTriggerEnter, &onTriggerEnter);
	Systems::IGlobalEventsSystem::Get()->bind(Events::Global::Update, &onUpdateListener);
	return true;
}

const std::string_view &EnemyBehavior::NameClass() const {
	static constexpr std::string_view className = NAMEOF(Components::Behaviors::EnemyBehavior);
	return className;
}

bool EnemyBehavior::Read(Stream &stream) {
	if(!Component::Read(stream)) {
		return false;
	}
	stream.Read("ContactDamage", contactDamage);
	stream.Read("PlayerContactDamage", playerContactDamage);
	stream.Read("Speed", speed);

	if(stream.ReadVec3("TargetDirection", targetDirection)) {
		const float len = glm::length(targetDirection);
		if(len > 1e-6f) {
			targetDirection /= len;
		} else {
			LOG_WARNING("EnemyBehavior: TargetDirection is near-zero, defaulting to (1,0,0)");
			targetDirection = glm::vec3(1.0f, 0.0f, 0.0f);
		}
	}
	// use temp variable to accumulate flags, only overwrite default if EnemyFlags is actually present in JSON
	EnemyFlags tempFlags = EnemyFlags::None;
	bool anyFlagRead = false;
	stream.ReadArray("EnemyFlags", [&] () {
		std::string flagStr;
		if(stream.Read("Flag", flagStr)) {
			anyFlagRead = true;
			if(flagStr == "BackAndForth") tempFlags |= EnemyFlags::BackAndForth;
			else if(flagStr == "Flying")       tempFlags |= EnemyFlags::Flying;
			else if(flagStr == "Stationary")   tempFlags |= EnemyFlags::Stationary;
			else if(flagStr == "Shooting")     tempFlags |= EnemyFlags::Shooting;
			else if(flagStr == "Boss")         tempFlags |= EnemyFlags::Boss;
			else LOG_WARNING("Unknown EnemyFlag: {}", flagStr);
		}
		});
	if(anyFlagRead) {
		enemyFlags = tempFlags;
	}
	return true;
}
bool EnemyBehavior::Update(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &) {
	if(m_transform == nullptr) {
		m_transform = Parent()->Get<Components::Transform>();
	}
	if(m_body == nullptr) {
		m_body = Parent()->Get<Components::PhysicsBody>();
		if(m_body != nullptr) {
			m_body->SetVelocity(targetDirection * speed);
		}
	}
	if(m_body == nullptr) {
		return true;
	}

	if(HasFlag(EnemyFlags::BackAndForth)) {
		if(m_body->isTouchingLeftWall || m_body->isTouchingRightWall) {
			targetDirection.x = -targetDirection.x;
		}
		m_body->SetVelocity(glm::vec3(targetDirection.x * speed, m_body->GetVelocity().y, 0.0f));
	}

	if(HasFlag(EnemyFlags::Flying)) {
		if(m_body->isTouchingLeftWall || m_body->isTouchingRightWall) {
			targetDirection.x = -targetDirection.x;
		}
		if(m_body->isTouchingCeiling || m_body->isGrounded)
		{
			targetDirection.y = -targetDirection.y;
			//just to avoid level issue bug for now
			//m_body->SetVelocity(glm::vec3(m_body->GetVelocity().x, targetDirection.y * speed, 0));
		}
		// technically this is the correct version, but in order to to avoid level flip issue, abandon this. it's hard to explain......
		m_body->SetVelocity(glm::vec3(targetDirection.x * speed, targetDirection.y * speed, 0.0f));
		//just to avoid level flip issue for now
		//m_body->SetVelocity(glm::vec3(targetDirection.x * speed, m_body->GetVelocity().y, 0.0f));
	}


	return true;
}
bool EnemyBehavior::OnTriggerEnter(const IEvent<Events::EventArgs> *, const Events::EventArgs &args) {
	Entity *other = dynamic_cast<const Events::TriggerEventArgs &>(args).otherEntity;
	if(!other) {
		return true;
	}
	if(other->Get<EnemyBehavior>() != nullptr) {
		return true;  // Ignore collisions with other enemies
	}

	// ── OnTriggerEnter / collision handler ──────────────────────────

	if(auto *bullet = other->Get<BulletBehavior>()) {
		if(!bullet->IsEnemyBullet()) {
			if(auto *myHealth = Parent()->Get<Components::HealthComponent>()) {
				// God mode: player bullets deal 2x damage to enemies
				const bool godMode = Systems::ICheatSystem::Get() && Systems::ICheatSystem::Get()->IsGodMode();
				const int damageAmount = godMode ? bullet->GetDamage() * 2 : bullet->GetDamage();
				myHealth->TakeDamage(damageAmount);
			}

			if(auto *knockback = Parent()->Get<Components::KnockbackComponent>()) {
				float dx = Parent()->GetTransform()->GetPosition().x - other->GetTransform()->GetPosition().x;
				float dirX = (dx >= 0.0f) ? 1.0f : -1.0f;
				knockback->Activate(dirX);
			}

			if(bullet->ShouldDestroyOnImpact()) {
				other->Destroy();
			}
		}
		return true;
	}

	if(auto *playerHealth = other->Get<Components::HealthComponent>()) {
		// God mode: player is invulnerable to enemy contact damage
		const bool godMode = Systems::ICheatSystem::Get() && Systems::ICheatSystem::Get()->IsGodMode();
		if(!godMode) {
			playerHealth->TakeDamage(playerContactDamage);
		}

		if(auto *myHealth = Parent()->Get<Components::HealthComponent>()) {
			myHealth->TakeDamage(contactDamage);
		}

		if(auto *knockback = Parent()->Get<Components::KnockbackComponent>()) {
			float dx = Parent()->GetTransform()->GetPosition().x - other->GetTransform()->GetPosition().x;
			float dirX = (dx >= 0.0f) ? 1.0f : -1.0f;
			knockback->Activate(dirX);
		}
	}
	return true;
}

}
