// File Name:    Random.cpp
// Author(s):    main Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Utility for generating random numbers.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "Random.h"

#include <ctime>
#include <random>

namespace RassEngine {

Random::Random() : generator{} {
	generator.seed(static_cast<unsigned>(std::time(0)));
}

}
