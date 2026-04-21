// File Name:    Flipper.h
// Author(s):    main Niko Bekris, secondary Boyuan Hu, Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Component executing the gravity-flip mechanic.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <Cloneable.h>
#include <Component.h>
#include <Events/EventArgs.h>
#include <Events/EventListenerMethod.h>
#include <Events/GlobalEventArgs.h>
#include <Events/GlobalEventListener.h>
#include <IEvent.h>
#include <Stream.h>
#include <string>
#include <string_view>
#include <Utils.h>
#include <Entity.h>
namespace RassEngine::Systems {
class IInputSystem;
}

namespace RassGame::Components {

// Forward declarations
class FlipOrigin;

class Flipper : public RassEngine::Cloneable<RassEngine::Component, Flipper> {
	static constexpr std::string_view FLIP_ENTITY_NAME = NAMEOF(FlipOrigin);
	static constexpr std::string_view DETECT_TRIGGER = "Player";
public:
	Flipper();
	Flipper(const Flipper &other);
	virtual ~Flipper() override;

	virtual bool Initialize() override;
	virtual const std::string_view &NameClass() const override;
	virtual bool Read(RassEngine::Stream &stream) override;
public:
	enum class Type {
		Both,
		UpsideDown,
		RightSideUp
	};

	inline Type GetType() const {
		return type;
	}
private:
	bool OnTriggerEnter(const RassEngine::IEvent<RassEngine::Events::EventArgs> *, const RassEngine::Events::EventArgs &args);
	bool OnTriggerExit(const RassEngine::IEvent<RassEngine::Events::EventArgs> *, const RassEngine::Events::EventArgs &args);
	bool OnUpdate(const RassEngine::IEvent<RassEngine::Events::GlobalEventArgs> *, const RassEngine::Events::GlobalEventArgs &);
	void LinkTextEntity();
private:
	Type type{Type::Both};
	std::string triggeredBy{"Player"};
	std::string textEntityName{"TipsTextEntity"};
	std::string flipAudioName;
	FlipOrigin *flipOrigin{nullptr};
	RassEngine::Entity *text{nullptr};
	RassEngine::Systems::IInputSystem *inputSystem{nullptr};
	bool playerInZone{false};
	bool entitiesLinked_{false};
	RassEngine::Events::EventListenerMethod<Flipper, RassEngine::Events::EventArgs> onTriggerEnter;
	RassEngine::Events::EventListenerMethod<Flipper, RassEngine::Events::EventArgs> onTriggerExit;
	RassEngine::Events::GlobalEventListener<Flipper> onUpdateListener;
};

}
