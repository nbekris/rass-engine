// File Name:    Precompiled.h
// Author(s):    main Taro Omiya, secondary Eric Fleegal, Steven Yacoub
// Course:       GAM541
// Project:      RASS
// Purpose:      Precompiled header for faster compilation.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

// Windows Macros
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define NOMINMAX				// Disable the Windows.h versions of min/max.
#include <Windows.h>

// GLM
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

// nlohmann jsaon parser
#include <nlohmann/json.hpp>

// Standard libraries
#include <algorithm>
#include <cmath>
#include <concepts>
#include <cstdio>
#include <format>
#include <functional>
#include <memory>
#include <print>
#include <string_view>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
