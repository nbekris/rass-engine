// File Name:    IEventArgs.h
// Author(s):    main Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Interface definition for event arguments.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <concepts>

namespace RassEngine {

struct IEventArgs {
	// Default methods
	inline IEventArgs() = default;
	inline virtual ~IEventArgs() = default;

private:
	// Remove the rest of the default functions
	IEventArgs(const IEventArgs &other) = delete;
	IEventArgs(IEventArgs &&other) noexcept = delete;
	IEventArgs &operator=(const IEventArgs &) = delete;
	IEventArgs &operator=(IEventArgs &&) noexcept = delete;
};

/// <summary>
/// Applies for any classes that publicly extends <see cref="EventArgs"/>.
/// </summary>
template<class T>
concept IsEventArgs = std::derived_from<T, IEventArgs>;

}
