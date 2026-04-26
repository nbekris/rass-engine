//------------------------------------------------------------------------------
//
// File Name:	TweenCurve.h
// Author(s):	taro.omiya
// Course:		CS529F25
// Project:		Project 7
// Purpose:		Class for finding in-betweening values (usually between 0 and 1)
//
// Copyright © 2025 DigiPen (USA) Corporation.
//
//------------------------------------------------------------------------------

#pragma once
#include <vector>

//------------------------------------------------------------------------------
// Namespace Declarations:
//------------------------------------------------------------------------------


namespace RassEngine {
// Forward Declarations:
class Stream;
class Vector2D;
class Color;

// Class Definition:
class TweenCurve {
public:
	enum class Type : int {
		Linear = 0,
		EaseInCircle,
		EaseOutCircle,
		Max
	};

private:
	typedef struct KeyFrame {
		Type type{Type::Linear};
		float time{0.f};
		float value{0.f};
	} KeyFrame;

public:
	static float Calculate(const Type &algorithm, float startValue, float endValue, float time);
	static Vector2D Calculate(const Type &algorithm, const Vector2D &startValue, const Vector2D &endValue, float time);
	static Color Calculate(const Type &algorithm, const Color &startValue, const Color &endValue, float time);

	TweenCurve() {};
	TweenCurve(const TweenCurve &other);
	virtual ~TweenCurve() {};

	float Calculate(float time) const;
	void Read(Stream &stream);

private:
	float startingValue{1.f};
	std::vector<KeyFrame> keyFrames;
};

}	// namespace
