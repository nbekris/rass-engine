// File Name:    Random.h
// Author(s):    main Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Utility for generating random numbers.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <random>
#include <numbers>

namespace RassEngine {

class Random {
public:
	Random();

	/// <summary>
	/// Gets a random value from min and max, inclusive.
	/// </summary>
	/// <typeparam name="T">Numeric type, e.g. int, float, etc.</typeparam>
	/// <param name="rangeMin">minimum value, inclusive</param>
	/// <param name="rangeMax">maximum value, inclusive</param>
	/// <returns>A random number</returns>
	template<typename T>
	static T range(T rangeMin, T rangeMax);

	template<typename T>
	static T angleRad();

private:
	inline static Random *get() {
		static std::unique_ptr<Random> instance;
		if(instance == nullptr) {
			instance = std::make_unique<Random>();
		}
		return instance.get();
	}

	// Mersenne Twister
	std::mt19937 generator;

	// Delete the unused operators
	Random(const Random &) = delete;
	Random(Random &&) noexcept = delete;
	Random &operator=(const Random &) = delete;
	Random &operator=(Random &&) noexcept = delete;
};

template<typename T>
inline T Random::range(T rangeMin, T rangeMax) {
	// Code taken from https://learn.microsoft.com/en-us/cpp/standard-library/random
	return std::uniform_int_distribution<T>(rangeMin, rangeMax)(get()->generator);
}

template<typename T>
inline T Random::angleRad() {
	return range(static_cast<T>(0), (std::numbers::pi_v<T> *static_cast<T>(2)));
}

template<>
inline float Random::range<float>(float rangeMin, float rangeMax) {
	return std::uniform_real_distribution<float>(rangeMin, rangeMax)(get()->generator);
}

template<>
inline double Random::range<double>(double rangeMin, double rangeMax) {
	return std::uniform_real_distribution<double>(rangeMin, rangeMax)(get()->generator);
}

template<>
inline long double Random::range<long double>(long double rangeMin, long double rangeMax) {
	return std::uniform_real_distribution<long double>(rangeMin, rangeMax)(get()->generator);
}

}
