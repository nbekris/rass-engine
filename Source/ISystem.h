// File Name:    ISystem.h
// Author(s):    main Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Interface for engine systems.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <concepts>
#include <type_traits>

#include "Object.h"

namespace RassEngine {

// Forward declaration
class ISystem;
namespace Systems {
class SystemsManager;
}

/// <summary>
/// Applies for any classes that publicly extends <see cref="ISystem"/>,
/// and is an interface.
/// </summary>
template<class T>
concept IsSystemInterface = std::is_abstract_v<T> && std::derived_from<T, ISystem>;

/// <summary>
/// Interface class that all systems need to extend.
/// </summary>
class ISystem : public Object {
public:
	// Default deconstructor
	virtual ~ISystem() = default;

	// The abstract functions
	/// <summary>
	/// Function called before destructor is called
	/// </summary>
	virtual void Shutdown() = 0;

protected:
	// Do not allow constructing an interface
	ISystem() = default;

private:
	// Remove the rest of the default functions
	ISystem(const ISystem &) = delete;
	ISystem(ISystem &&) noexcept = delete;
	ISystem &operator=(const ISystem &) = delete;
	ISystem &operator=(ISystem &&) noexcept = delete;
};

}
