//------------------------------------------------------------------------------
//
// File Name:	TweenCurve.cpp
// Author(s):	taro.omiya
// Course:		CS529F25
// Project:		Project 7
// Purpose:		Class for finding in-betweening values (usually between 0 and 1)
//
// Copyright © 2025 DigiPen (USA) Corporation.
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Includes:
//------------------------------------------------------------------------------

#include "Precompiled.h"
#include "TweenCurve.h"
#include "Utils.h"
#include "Stream.h"
#include "Vector2D.h"
#include "Color.h"

//------------------------------------------------------------------------------
// External Declarations:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Namespace Declarations:
//------------------------------------------------------------------------------


namespace RassEngine {
static const char *LOG_TWEEN_CURVE = "TweenCurve";
static const char *START = "Start";
static const char *KEYFRAMES = "KeyFrames";
static const char *TYPE = "Type";
static const char *TIME = "Time";
static const char *VALUE = "Value";

float Lerp(float start, float end, float time) {
	return (end - start) * time + start;
}

float EaseInCircle(float time) {
	// tweening code from https://easings.net/#easeInCirc
	return 1.f - std::sqrt(1.0f - std::pow(time, 2.0f));
}

float EaseOutCircle(float time) {
	// tweening code from https://easings.net/#easeOutCirc
	return std::sqrt(1.0f - std::pow(time - 1.0f, 2.0f));
}

float CalculateNewTime(const TweenCurve::Type &algorithm, float time) {
	// Choose a tweening algorithm
	switch(algorithm) {
	case TweenCurve::Type::EaseInCircle:
		return EaseInCircle(time);
		break;
	case TweenCurve::Type::EaseOutCircle:
		return EaseOutCircle(time);
		break;
	}
	return time;
}

float TweenCurve::Calculate(const Type &algorithm, float start, float end, float time) {
	// Update time
	time = CalculateNewTime(algorithm, time);

	// Lerp
	return Lerp(start, end, time);
}

Vector2D TweenCurve::Calculate(const Type &algorithm, const Vector2D &start, const Vector2D &end, float time) {
	// Update time
	time = CalculateNewTime(algorithm, time);

	// Lerp
	return Vector2D(Lerp(start.x, end.x, time), Lerp(start.y, end.y, time));
}

Color TweenCurve::Calculate(const Type &algorithm, const Color &start, const Color &end, float time) {
	// Update time
	time = CalculateNewTime(algorithm, time);

	// Lerp
	return Color(Lerp(start.r, end.r, time), Lerp(start.g, end.g, time), Lerp(start.b, end.b, time), Lerp(start.a, end.a, time));
}

TweenCurve::TweenCurve(const TweenCurve &other)
	: startingValue(other.startingValue) {
	keyFrames.reserve(other.keyFrames.size());
	for(const KeyFrame &key : other.keyFrames) {
		keyFrames.emplace_back(key);
	}
}

float CS529::TweenCurve::Calculate(float time) const {
	// Check if there are any keyframes
	if(keyFrames.size() == 0) {
		// If not, just return the starting value
		return startingValue;
	}

	// Setup loop variables
	float startTime = 0;
	float startValue = startingValue;
	auto nextKeyFrame = keyFrames.begin();

	// Find a keyframe time is on
	while((nextKeyFrame->time < time) && (nextKeyFrame != keyFrames.end())) {
		// Grab this keyframe's stats
		startTime = nextKeyFrame->time;
		startValue = nextKeyFrame->value;

		// before moving on to the next keyframe
		++nextKeyFrame;
	}

	// Check if we're on the last keyframe
	if(nextKeyFrame == keyFrames.end()) {
		// If so, just return the keyframe's value
		return nextKeyFrame->value;
	}

	// Normalize the time between startTime and nextKeyFrame->time
	time = (time - startTime) / (nextKeyFrame->time - startTime);

	// Choose a tweening algorithm
	return Calculate(nextKeyFrame->type, startValue, nextKeyFrame->value, time);
}

void TweenCurve::Read(Stream &stream) {
	// Read the starting value
	Utils::ReadOptionalAttribute(stream, START, startingValue, LOG_TWEEN_CURVE);

	// Go through the array of keyframes
	stream.ReadArray(KEYFRAMES, [this, &stream] () {
		KeyFrame key;

		// First, read the type of the keyframe
		int typeOrdinal = static_cast<int>(Type::Linear);
		Utils::ReadOptionalAttribute(stream, TYPE, typeOrdinal, LOG_TWEEN_CURVE);
		DRAGON_ASSERT(typeOrdinal < static_cast<int>(Type::Max), "TweenCurve::Read: unknown type read");
		key.type = static_cast<Type>(typeOrdinal);

		// Read the rest
		Utils::ReadOptionalAttribute(stream, TIME, key.time, LOG_TWEEN_CURVE);
		Utils::ReadOptionalAttribute(stream, VALUE, key.value, LOG_TWEEN_CURVE);

		// Add to the list
		keyFrames.emplace_back(key);
		});
}
}	// namespace
