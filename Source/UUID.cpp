// File Name:    UUID.cpp
// Author(s):    main Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Universally unique identifier generation and management.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "UUID.h"

#include <limits>
#include <utility>

#include "Random.h"

namespace RassEngine {

UUID::UUID()
	: id{Random::range(static_cast<Type>(0), std::numeric_limits<Type>().max())}
{}

UUID::UUID(UUID &&other) noexcept {
	std::swap(id, other.id);
}

}
