// File Name:    KnockbackComponent.h
// Author(s):    main Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Component applying knockback force to an entity.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once
#include <string_view>

#include "Component.h"
#include "Cloneable.h"
#include "Events/GlobalEventArgs.h"
#include "Events/GlobalEventListener.h"
#include "IEvent.h"
#include "Stream.h"

namespace RassEngine::Components {
class PhysicsBody;

class KnockbackComponent : public Cloneable<Component, KnockbackComponent> {
public:
	KnockbackComponent();
	KnockbackComponent(const KnockbackComponent &other);
	virtual ~KnockbackComponent() override;

	virtual bool Initialize() override;
	virtual const std::string_view &NameClass() const override;
	virtual bool Read(Stream &stream) override;

	void Activate(float directionX);
	bool IsActive() const { return isActive; }

private:
	bool OnFixedUpdate(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &);
	Events::GlobalEventListener<KnockbackComponent> onFixedUpdateListener;

	PhysicsBody *physics{nullptr};
	bool isActive{false};
	float timer{0.0f};
	float dirX{1.0f};

	float duration{0.25f};
	float forceX{6.0f};
	float forceY{2.5f};
};
}
