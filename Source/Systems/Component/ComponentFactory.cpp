// File Name:    ComponentFactory.cpp
// Author(s):    main Steven Yacoub, secondary Niko Bekris, Taro Omiya, Boyuan Hu
// Course:       GAM541
// Project:      RASS
// Purpose:      Factory for creating and registering components.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "ComponentFactory.h"

#include <string>
#include <string_view>

#include "Component.h"
#include "IComponentFactory.h"
#include "Systems/Logging/ILoggingSystem.h"
#include "Utils.h"

#include "Components/Animation.h"
#include "Components/AnimationController.h"
#include "Components/AudioSource.h"
#include "Components/Camera.h"
#include "Components/Collider.h"
#include "Components/DebugDrawer.h"
#include "Components/Movement.h"
#include "Components/PhysicsBody.h"
#include "Components/Sprite.h"
#include "Components/CursorMode.h"
#include "Components/Button.h"
#include "Components/MusicFader.h"
#include "Components/Transform.h"
#include "Components/Trigger.h"
#include "Components/TileMap.h"
#include "Components/ShootingController.h"
#include "Components/Behaviors/SinRotate.h"
#include "Components/Behaviors/SinScale.h"
#include "Components/Behaviors/SinTranslate.h"
#include "Components/Behaviors/BulletBehavior.h"
#include "Components/Behaviors/EnemyBehavior.h"
#include "Components/HealthComponent.h"
#include "Components/KnockbackComponent.h"
#include "Components/Pushable.h"
#include "Components/PushPullController.h"
#include "Components/MenuMouseFollower.h"
#include "Components/SpriteFader.h"
#include "Components/MenuNavigator.h"

//CrossFadeMusic
namespace RassEngine::Systems {
	ComponentFactory::ComponentFactory() {
		//Components
		Register("Animation", []()             -> Component* { return new Components::Animation(); });
		Register("AnimationController", []()   -> Component* { return new Components::AnimationController(); });
		Register("AudioSource", []()           -> Component* { return new Components::AudioSource(); });
		Register("Camera", []()                -> Component* { return new Components::Camera(); });
		Register("Collider", []()              -> Component* { return new Components::Collider(); });
		Register("MusicFader", [] ()           -> Component* { return new Components::MusicFader(); });
		Register("SpriteFader", [] ()          -> Component* { return new Components::SpriteFader(); });
		Register("DebugDrawer", []()           -> Component* { return new Components::DebugDrawer(); });
		Register("Movement", []()              -> Component* { return new Components::Movement(); });
		Register("PhysicsBody", []()           -> Component* { return new Components::PhysicsBody(); });
		Register("Sprite", []()                -> Component* { return new Components::Sprite(); });
		Register("CursorMode", [] ()           -> Component* { return new Components::CursorMode(); });
		Register("Button", [] ()               -> Component* { return new Components::Button(); });
		Register("Transform", []()             -> Component* { return new Components::Transform(); });
		Register("Trigger", [] ()              -> Component* { return new Components::Trigger(); });
		Register("TileMap", [] ()              -> Component* { return new Components::TileMap(); });
		Register("ShootingController", [] ()   -> Component* { return new Components::ShootingController(); });
		Register("HealthComponent", [] ()      -> Component* { return new Components::HealthComponent(); });
		Register("KnockbackComponent", [] ()   -> Component* { return new Components::KnockbackComponent(); });
		Register("Pushable", [] ()             -> Component* { return new Components::Pushable(); });
		Register("PushPullController", [] ()   -> Component* { return new Components::PushPullController(); });
		Register("MenuMouseFollower", [] ()    -> Component* { return new Components::MenuMouseFollower(); });
		Register("MenuNavigator", [] ()        -> Component* { return new Components::MenuNavigator(); });
		//Behaviors
		Register("SinRotate", []()             -> Component* { return new Components::Behaviors::SinRotate(); });
		Register("SinScale", []()              -> Component* { return new Components::Behaviors::SinScale(); });
		Register("SinTranslate", [] ()         -> Component* { return new Components::Behaviors::SinTranslate(); });
		Register("BulletBehavior", [] ()       -> Component* { return new Components::Behaviors::BulletBehavior(); });
		Register("EnemyBehavior", [] ()        -> Component* { return new Components::Behaviors::EnemyBehavior(); });
	}

	ComponentFactory::~ComponentFactory() {
		registry.clear();
	}

	bool ComponentFactory::Initialize() {
		LOG_INFO("ComponentFactory initialized");
		return true;
	}

	void ComponentFactory::Shutdown() {
		// Do nothing
	}

	const std::string_view &ComponentFactory::NameClass() const {
		static constexpr std::string_view className = NAMEOF(RassEngine::Systems::IComponentFactory);
		return className;
	}

	bool ComponentFactory::Register(const std::string &componentName, componentConstructor constructor) {
		if(IsRegistered(componentName)) {
			LOG_DEBUG("DEBUG: Attempted to register duplicate component name: {}", componentName);
			return false;
		}

		registry[componentName] = constructor;
		return true;
	}

	Component* ComponentFactory::Create(const std::string &compName) const {
		auto component = registry.find(compName);

		if(component != registry.end()) {
			return component->second();
		}

		return nullptr;
	}
}
