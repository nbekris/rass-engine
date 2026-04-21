// File Name:    BulletBehavior.h
// Author(s):    main Boyuan Hu, secondary Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Behavior component controlling bullet movement and lifetime.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <glm/vec3.hpp>
#include <string_view>
#include <unordered_set>
#include "Component.h"
#include "Cloneable.h"
#include "Events/GlobalEventListener.h"
//#include "Events/EventListenerLambda.h"  
#include "IEvent.h"
#include "Stream.h"

namespace RassEngine::Events {
struct GlobalEventArgs;
}

namespace RassEngine::Components::Behaviors {

class BulletBehavior : public Cloneable<Component, BulletBehavior> {
public:
	BulletBehavior();
	BulletBehavior(const BulletBehavior &other);
	virtual ~BulletBehavior() override;

	virtual bool Initialize() override;
	virtual const std::string_view &NameClass() const override;
	virtual bool Read(Stream &stream) override;
	float GetSpeed() const {
		return speed;
	}
	int   GetDamage() const {
		return damage;
	}
	bool  IsEnemyBullet() const {
		return isEnemyBullet;
	}
	bool  ShouldDestroyOnImpact() const {
		return destroyOnImpact;
	}
private:
	bool Update(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &);
private:
	Events::GlobalEventListener<BulletBehavior> onUpdateListener;

	int damage{1};
	bool destroyOnImpact{true};
	bool canCrossColliders{false};
	bool isEnemyBullet{false};
	bool destroysTiles{false};
	std::string tileDestroySoundFile;
	float lifetime{5.0f};
	int bounces{0};
	float speed{10.0f};
	float timer{0.0f};
	float cachedProbeX{0.3f};
	float cachedProbeY{0.3f};
	// Used for rotation-independent tile hit detection:
	glm::vec2 prevVelocity{0.0f, 0.0f};
	std::unordered_set<unsigned> destroyableTileTypes{};
};

}
