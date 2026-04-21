// File Name:    HealthComponent.cpp
// Author(s):    main Boyuan Hu, secondary Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Component tracking and managing entity health.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "Components/HealthComponent.h"

#include "Component.h"
#include "Components/Sprite.h"
#include "Components/Movement.h"
#include "Entity.h"
#include "Events/Global.h"
#include "Events/GlobalEventArgs.h"
#include "IEvent.h"
#include "Cloneable.h"
#include "Systems/GlobalEvents/IGlobalEventsSystem.h"
#include "Systems/Time/ITimeSystem.h"
#include "Systems/Logging/ILoggingSystem.h"
#include "Systems/Scene/ISceneSystem.h"
#include "Events/AudioEvents.h"
#include "Systems/Entity/IEntityFactory.h"

namespace RassEngine::Components {
HealthComponent::HealthComponent()
	: Cloneable<Component, HealthComponent>{}
	, onUpdateListener{this, &HealthComponent::Update} {}

HealthComponent::HealthComponent(const HealthComponent &other)
	: Cloneable<Component, HealthComponent>{other}
	, onUpdateListener{this, &HealthComponent::Update}
	, health{other.health}
	, maxHealth{other.maxHealth}
	, isPlayer{other.isPlayer}
	, damageAudioFile{other.damageAudioFile}
{
}

HealthComponent::~HealthComponent() {
	if(Systems::IGlobalEventsSystem::Get() == nullptr) {
		return;
	}
	Systems::IGlobalEventsSystem::Get()->unbind(Events::Global::Update, &onUpdateListener);
}

bool HealthComponent::Initialize() {
	if(Systems::IGlobalEventsSystem::Get() == nullptr) {
		LOG_ERROR("Cannot initialize HealthComponent: {} is not registered", NAMEOF(Systems::IGlobalEventsSystem));
		return false;
	}
	//if(isPlayer) {
	//	LinkHeartEntities();
	//	UpdateHeartSprites();
	//}
	Systems::IGlobalEventsSystem::Get()->bind(Events::Global::Update, &onUpdateListener);
	RassEngine::Events::AudioEvents::LoadSound(damageAudioFile);
	return true;
}

bool HealthComponent::Read(Stream &stream) {
	//stream.Read("ShowFPS", showFPS);
	stream.Read("MaxHealth", maxHealth);
	stream.Read("Health", health);
	stream.Read("FlashDuration", flashDuration);
	stream.Read("IsPlayer", isPlayer);
	stream.Read("DamageAudioFile", damageAudioFile);
	if(health > maxHealth) {
		health = maxHealth;
	}
	return true;
}
bool HealthComponent::TakeDamage(int damage) {
	if(damage < 0) {
		LOG_WARNING("{}: TakeDamage called with negative damage value {}", NameClass(), damage);
		return false;
	}
	if(isInvincible) {
		return false;
	}
	health -= damage;
	if(health < 0) {
		health = 0;
	}
	flashTimer = flashDuration;
	UpdateSpriteColor();
	RassEngine::Events::AudioEvents::PlaySFXRandomPitch(damageAudioFile);
	if(isPlayer) {
		UpdateHeartSprites();
		isInvincible = true;
		invincibilityTimer = invincibilityDuration;
	}
	if(health == 0) {
		OnDeath();
	}
	return true;
}
void HealthComponent::Heal(int amount) {
	if(amount < 0) {
		LOG_WARNING("{}: Heal called with negative amount value {}", NameClass(), amount);
		return;
	}
	health += amount;
	if(health > maxHealth) {
		health = maxHealth;
	}
	UpdateSpriteColor();
	if(isPlayer) {
		UpdateHeartSprites();
	}
}
bool HealthComponent::LinkHeartEntities() {
	if(!isPlayer) {
		LOG_WARNING("{}: LinkHeartEntities called on non-player entity", NameClass());
		return false;
	}

	auto *sceneSystem = Systems::ISceneSystem::Get();
	if(sceneSystem == nullptr) {
		return false;
	}

	auto *entityFactory = Systems::IEntityFactory::Get();
	if(entityFactory == nullptr) {
		LOG_ERROR("{}: IEntityFactory is not registered", NameClass());
		return false;
	}

	for(auto *heartEntity : heartEntities) {
		if(heartEntity != nullptr && !heartEntity->IsDestroyed()) {
			heartEntity->Destroy();
		}
	}
	heartEntities.clear();

	const int heartCount = (maxHealth + 1) / 2;

	for(int i = 0; i < heartCount; ++i) {
		auto heartEntity = entityFactory->Build(heartEntityPath);
		if(heartEntity == nullptr) {
			LOG_WARNING("{}: Failed to build heart entity '{}'", NameClass(), heartEntityPath);
			continue;
		}

		auto *transform = heartEntity->GetTransform();
		if(transform != nullptr) {
			transform->SetLocalPosition(firstHeartPos + glm::vec3(static_cast<float>(i) * heartSpacing, 0.0f, 0.0f));
		}

		Entity *rawPtr = heartEntity.get();
		sceneSystem->AddEntity(std::move(heartEntity));
		heartEntities.push_back(rawPtr);

	}

	return !heartEntities.empty();
}
bool HealthComponent::SetHeartsSprites(int heartIndex, const std::string &texturePath) {
	if(heartIndex < 0 || heartIndex >= static_cast<int>(heartEntities.size())) {
		return false;
	}
	Entity *heartEntity = heartEntities[heartIndex];
	if(heartEntity == nullptr || heartEntity->IsDestroyed()) {
		return false;
	}
	auto *heartSprite = heartEntity->Get<Sprite>();
	if(heartSprite == nullptr) {
		return false;
	}
	heartSprite->SetTexture(texturePath, false);
	return true;
}
void HealthComponent::UpdateHeartSprites() {
	for(int i = 0; i < static_cast<int>(heartEntities.size()); ++i) {
		// one heart represents 2 HP, so calculate the HP for this heart index
		const int hpForThisHeart = health - i * 2;
		const std::string *texturePath;
		if(hpForThisHeart >= 2) {
			texturePath = &fullheartTexturePath;   // full
		} else if(hpForThisHeart == 1) {
			texturePath = &halfheartTexturePath;   // half
		} else {
			texturePath = &emptyheartTexturePath;  // empty
		}
		SetHeartsSprites(i, *texturePath);
	}
}
void HealthComponent::OnDeath() {
	LOG_INFO("{}: Died", NameClass());
	if(!isPlayer)
		Parent()->Destroy();
	else
	{
		auto* movement = Parent()->Get<Movement>();
		if(movement != nullptr) {
			health = maxHealth;
			UpdateHeartSprites();
			movement->RespawnAtCheckpoint();
		}
		else {
			RassEngine::Systems::ISceneSystem::Get()->Restart();
		}

		return;
	}
}

const std::string_view &HealthComponent::NameClass() const {
	static constexpr std::string_view className = NAMEOF(RassEngine::Components::HealthComponent);
	return className;
}

void HealthComponent::UpdateSpriteColor() {
	if(!sprite) {
		sprite = Parent()->Get<Sprite>();
	}
	if(!sprite) return;

	if(flashTimer > 0.0f) {
		sprite->SetColor(1.0f, 0.0f, 0.0f);
	} else {
		sprite->SetColor(1.0f, 1.0f, 1.0f);
	}
	if(isPlayer) {
		if(invincibilityTimer > 0.0f) {
			sprite->Alpha(0.5f);
		} else {
			sprite->Alpha(1.0f);
		}
	}
}

bool HealthComponent::Update(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &args) {

	if(isPlayer && !heartsLinked) {
		LinkHeartEntities();
		UpdateHeartSprites();
		heartsLinked = true;
	}

	if(flashTimer > 0.0f) {
		flashTimer -= Systems::ITimeSystem::Get()->GetDeltaTimeSec();
		if(flashTimer <= 0.0f) {
			flashTimer = 0.0f;
			UpdateSpriteColor();

		}
	}
	if(isPlayer) {
		if(invincibilityTimer > 0.0f) {
			invincibilityTimer -= Systems::ITimeSystem::Get()->GetDeltaTimeSec();
			if(invincibilityTimer <= 0.0f) {
				invincibilityTimer = 0.0f;
				isInvincible = false;
				UpdateSpriteColor();
			}
		}
	}
	return true;
}
}

