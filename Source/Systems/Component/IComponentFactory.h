// File Name:    IComponentFactory.h
// Author(s):    main Steven Yacoub, secondary Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Factory for creating and registering components.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <functional>
#include <string>

#include "Systems/GlobalEvents/IGlobalSystem.h"
#include "Component.h"

namespace RassEngine::Systems {

class IComponentFactory : public IGlobalSystem<IComponentFactory> {

	public:
		virtual bool Register(const std::string &componentName, std::function<Component* ()> componentConstructor) = 0;
		virtual Component* Create(const std::string &componentName) const = 0;
		virtual bool IsRegistered(const std::string &componentName) const = 0;
	};

};
