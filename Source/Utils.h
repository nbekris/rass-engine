// File Name:    Utils.h
// Author(s):    main Taro Omiya, secondary Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      General utility functions and helpers.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <concepts>
#include <string>
#include <string_view>

namespace RassEngine {

// Concepts code borrowed from:
// https://medium.com/@nerudaj/dependency-injection-containers-c-and-a-ton-of-metaprogramming-2fb82e56bf6c
// and
// https://github.com/nerudaj/CppDic/blob/main/lib/include/cppdic/ServiceProviderBuilder.hpp

/// <summary>
/// Applies for any concrete classes extending another.
/// </summary>
template<class T, class I>
concept IsDerived = !std::is_abstract_v<T> && std::derived_from<T, I>;

/// @brief Converts the name of the identifier into a char[]
#define NAMEOF(name) #name

/// @brief Removes the namespace from a class name, if it exists.  Otherwise, returns the name as is.
static std::string RemoveNamespace(const std::string_view &nameClass) {
	std::string toReturn{nameClass};

	// Check if there's a namespace in the class name
	const auto index = toReturn.find_last_of(':');
	if(index == toReturn.npos) {
		// If not, just return the name as is
		return toReturn;
	}

	// Otherwise, return the substring after the last namespace
	return toReturn.substr(index + 1);
}

}
