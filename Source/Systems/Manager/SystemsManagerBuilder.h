// File Name:    SystemsManagerBuilder.h
// Author(s):    main Taro Omiya, secondary Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Builder for configuring and constructing the SystemsManager.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <memory>

#include "SystemsManager.h"
#include "ISystem.h"
#include "Utils.h"

namespace RassEngine::Systems {

/// <summary>
/// Builder for class <see cref="System"/>
/// </summary>
/// <example><code>
/// // Construct the builder
/// SystemBuilder builder;
/// 
/// // Then construct the systems manager with the builder
/// std::unique_ptr<Systems> systems =
///   // To register a System with a default constructor, just use the line below
///	  builder.Register<Interface, Concrete>()
///   // Can also overwrite previous insert with mock types like below
///   .Register<Interface>(std::make_unique<MockConcrete>())
///   // Last line should always be build
///   .build();
/// </code></example>
class SystemsManagerBuilder {
public:
	SystemsManagerBuilder();
	virtual ~SystemsManagerBuilder() = default;

	/// <summary>
	/// Maps a concrete instance of <paramref value="I"/> to the interface.
	/// </summary>
	/// <typeparam name="I">The "key" to call in <see cref="SystemsManager.get<T>()"/></typeparam>
	/// <typeparam name="C">Concrete implementation of I</typeparam>
	/// <returns>This builder</returns>
	template<IsSystemInterface I, class C, typename... _Types>
		requires IsDerived<C, I>
	inline SystemsManagerBuilder &Register(_Types&&... _Args) {
		return Register<I>(std::make_unique<C>(_Args...));
	}

	/// <summary>
	/// Maps an instance of <paramref value="I"/> to the interface.
	/// </summary>
	/// <typeparam name="I">The "key" to call in <see cref="SystemsManager.get<T>()"/></typeparam>
	/// <param name="instance">A concrete instance of I</param>
	/// <returns>This builder</returns>
	/// <remarks>
	/// This function *will* move the ownership of <paramref name="instance"/>
	/// to the built Systems manager.
	/// </remarks>
	template<IsSystemInterface I>
	SystemsManagerBuilder &Register(std::unique_ptr<I> &&instance) {
		toBuild->Register(std::move(instance));
		return *this;
	}

	/// <summary>
	/// Maps an instance of <paramref value="I"/> to the interface.
	/// </summary>
	/// <typeparam name="I">The "key" to call in <see cref="SystemsManager.get<T>()"/></typeparam>
	/// <param name="instance">A concrete instance of I</param>
	/// <param name="priority">Indicates the order in which system will be destroyed. Lower number = destroyed later.</param>
	/// <returns>This builder</returns>
	/// <remarks>
	/// This function *will* move the ownership of <paramref name="instance"/>
	/// to the built Systems manager.
	/// </remarks>
	template<IsSystemInterface I>
	SystemsManagerBuilder &Register(std::unique_ptr<I> &&instance, SystemsManager::PriorityType priority) {
		toBuild->Register(std::move(instance), priority);
		return *this;
	}

	/// <summary>
	/// Removes an instance of <paramref value="I"/> from the map.
	/// </summary>
	/// <typeparam name="I">The "key" to call in <see cref="SystemsManager.get<T>()"/></typeparam>
	/// <returns>This builder</returns>
	template<IsSystemInterface I>
	SystemsManagerBuilder &Unregister() {
		toBuild->Unregister();
		return *this;
	}

	/// <summary>
	/// Initializes all <see cref="ISystem"/>s, and returns the manager.
	/// </summary>
	/// <returns>Fully-initialized <see cref="System"/></returns>
	std::unique_ptr<SystemsManager> build();

private:
	// Remove the rest of the default functions
	SystemsManagerBuilder(const SystemsManagerBuilder &) = delete;
	SystemsManagerBuilder(SystemsManagerBuilder &&) noexcept = delete;
	SystemsManagerBuilder &operator=(const SystemsManagerBuilder &) = delete;
	SystemsManagerBuilder &operator=(SystemsManagerBuilder &&) noexcept = delete;

	// Member variables
	std::unique_ptr<SystemsManager> toBuild;
};

}
