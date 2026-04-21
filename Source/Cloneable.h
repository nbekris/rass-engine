// File Name:    Cloneable.h
// Author(s):    main Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Interface for cloneable objects.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <memory>
#include <concepts>

#include "ICloneable.h"

namespace RassEngine {

// Inspired by https://katyscode.wordpress.com/2013/08/22/c-polymorphic-cloning-and-the-crtp-curiously-recurring-template-pattern/
// Also inspired by https://stackoverflow.com/a/16605017
/// <summary>
/// Interface for implementing method <see cref="ICloneable<T>::Clone()"/>
/// </summary>
/// <typeparam name="B">Type to clone to</typeparam>
/// <typeparam name="Derived">Class deriving of B</typeparam>
template<class B, class D> requires std::derived_from<B, ICloneable<B>>
class Cloneable : public B {
public:
	using B::B;

	virtual ~Cloneable() override {};

	/// <summary>
	/// Creates a new instance of <typeparam name="T"/>
	/// </summary>
	inline virtual std::unique_ptr<B> Clone() const override {
		return std::make_unique<D>(static_cast<D const &>(*this));
	}
};

}
