// File Name:    ComponentFactory.h
// Author(s):    main Steven Yacoub, secondary Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Factory for creating and registering components.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once
#include "Precompiled.h"
#include "Component.h"
#include "IComponentFactory.h"
#include <functional>
#include <string_view>
#include <string>
#include <unordered_map>

namespace RassEngine::Systems {

	class ComponentFactory : public IComponentFactory{
		private:
			typedef std::function<Component* ()> componentConstructor;

		public:
			ComponentFactory();
			~ComponentFactory();

			virtual bool Initialize() override;

			virtual void Shutdown() override;

			virtual const std::string_view &NameClass() const override;

			virtual bool Register(const std::string &componentName, componentConstructor constructor) override;

			Component* Create(const std::string &componentName) const override;

			bool IsRegistered(const std::string &componentName) const {
				return registry.find(componentName) != registry.end();
			}

		private:
			std::unordered_map<std::string, componentConstructor> registry;

	};
}

