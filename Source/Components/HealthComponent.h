// File Name:    HealthComponent.h
// Author(s):    main Boyuan Hu, secondary Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Component tracking and managing entity health.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once
#include <string_view>

#include "Component.h"
#include "Cloneable.h"
#include "Events/GlobalEventArgs.h"
#include "Events/GlobalEventListener.h"
#include "IEvent.h"
#include "Components/Sprite.h"
#include "Stream.h"
#include <glm/glm.hpp>
namespace RassEngine {
class Stream;
}

namespace RassEngine::Components {
class HealthComponent : public Cloneable<Component, HealthComponent> {
public:
	HealthComponent();
	HealthComponent(const HealthComponent &other);
	virtual ~HealthComponent() override;

	virtual bool Initialize() override;
	virtual const std::string_view &NameClass() const override;
	virtual bool Read(Stream &stream) override;
	int GetHealth() const {
		return health;
	}
	int GetMaxHealth() const {
		return maxHealth;
	}
	bool TakeDamage(int damage);
	void Heal(int amount);
	bool isPlayer{false};
private:
	bool Update(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &);
	Events::GlobalEventListener<HealthComponent> onUpdateListener;
	void UpdateSpriteColor();
	void OnDeath();
	bool LinkHeartEntities();
	bool SetHeartsSprites(int heartIndex, const std::string &texturePath);
	void UpdateHeartSprites();
	
	Sprite *sprite{nullptr};
	int health{100};
	int maxHealth{100};
	float flashTimer{0.0f};
	float flashDuration{0.3f};
	std::string damageAudioFile;
	float invincibilityTimer{0.0f};
	float invincibilityDuration{1.0f};
	bool isInvincible{false};

	bool heartsLinked{false};
	std::vector<Entity *> heartEntities{};
	std::string heartEntityPath{"HeartHUD"};
	std::string fullheartTexturePath{"Assets/Textures/hud_heart.png"};
	std::string halfheartTexturePath{"Assets/Textures/hud_heart_half.png"};
	std::string emptyheartTexturePath{"Assets/Textures/hud_heart_empty.png"};
	glm::vec3 firstHeartPos{-1.4f, 0.8f, -0.9f};
	float heartSpacing{0.2f};
};
}
