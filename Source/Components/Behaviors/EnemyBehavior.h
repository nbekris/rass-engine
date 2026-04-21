// File Name:    EnemyBehavior.h
// Author(s):    main Boyuan Hu, secondary Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Behavior component for enemy AI logic.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <string_view>

#include "Component.h"
#include "Cloneable.h"
#include "Events/EventArgs.h"
#include "Events/EventListenerMethod.h"
#include "Events/GlobalEventListener.h"
#include "IEvent.h"
#include "Stream.h"
#include "Components/PhysicsBody.h"
#include "Components/Transform.h"
#include <glm/vec3.hpp>
namespace RassEngine::Components::Behaviors {

class EnemyBehavior : public Cloneable<Component, EnemyBehavior> {
public:
	enum class EnemyFlags : unsigned int {
		None = 0,
		BackAndForth = 1 << 0,  // 1
		Flying = 1 << 1,  // 2
		Stationary = 1 << 2,  // 4
		Shooting = 1 << 3,  // 8
		Boss = 1 << 4,  // 16
	};
	EnemyBehavior();
	EnemyBehavior(const EnemyBehavior &other);
	virtual ~EnemyBehavior() override;

	virtual bool Initialize() override;
	virtual const std::string_view &NameClass() const override;
	virtual bool Read(Stream &stream) override;
	bool HasFlag(EnemyFlags flag) const {
		return (static_cast<unsigned int>(enemyFlags) & static_cast<unsigned int>(flag)) != 0;
	}
private:
	bool OnTriggerEnter(const IEvent<Events::EventArgs> *, const Events::EventArgs &);

private:
	Events::EventListenerMethod<EnemyBehavior, Events::EventArgs> onTriggerEnter;
	bool Update(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &);
	Events::GlobalEventListener<EnemyBehavior> onUpdateListener;
	//damage to enemy itself
	int contactDamage{1};
	//damage to player on contact
	int playerContactDamage{1};
	EnemyFlags enemyFlags{EnemyFlags::BackAndForth};
	PhysicsBody *m_body{nullptr};
	Transform *m_transform{nullptr};
	glm::vec3 targetDirection{1.0f, 0.0f, 0.0f};
	glm::vec3 prevVelocity{0.0f, 0.0f, 0.0f};
	float speed{1.0f};
};
inline EnemyBehavior::EnemyFlags operator|(EnemyBehavior::EnemyFlags a, EnemyBehavior::EnemyFlags b) {
	return static_cast<EnemyBehavior::EnemyFlags>(
		static_cast<unsigned int>(a) | static_cast<unsigned int>(b));
}
inline EnemyBehavior::EnemyFlags operator&(EnemyBehavior::EnemyFlags a, EnemyBehavior::EnemyFlags b) {
	return static_cast<EnemyBehavior::EnemyFlags>(
		static_cast<unsigned int>(a) & static_cast<unsigned int>(b));
}
inline EnemyBehavior::EnemyFlags &operator|=(EnemyBehavior::EnemyFlags &a, EnemyBehavior::EnemyFlags b) {
	return a = a | b;
}
}
