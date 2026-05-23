// File Name:    ParentTo.h
// Author(s):    main Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Component that sets the entity's parent on game start
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <glm/glm.hpp>
#include <string>
#include <string_view>

#include <Cloneable.h>
#include <Component.h>
#include <Events/GlobalEventArgs.h>
#include <Events/GlobalEventListener.h>
#include <IEvent.h>
#include <Stream.h>

namespace RassGame::Components {

class ParentTo : public RassEngine::Cloneable<RassEngine::Component, ParentTo> {
public:
	ParentTo();
	ParentTo(const ParentTo &other);
	virtual ~ParentTo() override;

	virtual bool Initialize() override;
	virtual const std::string_view &NameClass() const override;
	virtual bool Read(RassEngine::Stream &stream) override;

private:
	bool OnStart(const RassEngine::IEvent<RassEngine::Events::GlobalEventArgs> *, const RassEngine::Events::GlobalEventArgs &);

	std::string entityName{""};

	bool setPosition{false};
	glm::vec3 localPosition{};

	bool setRotation{false};
	float localRotation{};

	bool setScale{false};
	glm::vec3 localScale{};

	RassEngine::Events::GlobalEventListener<ParentTo> onStartListener;
};

}
