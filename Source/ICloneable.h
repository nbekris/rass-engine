// File Name:    ICloneable.h
// Author(s):    main Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Interface for cloneable objects.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <memory>

namespace RassEngine {

/// <summary>
/// Interface for implementing method <see cref="ICloneable<T>::Clone()"/>
/// </summary>
/// <typeparam name="T">Type to clone to</typeparam>
template<class T>
class ICloneable {
public:
	virtual ~ICloneable() = default;

	/// <summary>
	/// Creates a new instance of <typeparam name="T"/>
	/// </summary>
	virtual std::unique_ptr<T> Clone() const = 0;
};

}
