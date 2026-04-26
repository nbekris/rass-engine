#include "Precompiled.h"

#include <cmath>
#include <memory>
#include <string>
#include <string_view>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "TweenCurve.h"
#include "Utils.h"
#include "Stream.h"
#include "Systems/Logging/ILoggingSystem.h"

namespace RassEngine {
static const char *START = "Start";
static const char *KEYFRAMES = "KeyFrames";
static const std::string TYPE = Utils::RemoveNamespace(NAMEOF(TweenCurve::Type));
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

glm::vec2 TweenCurve::Calculate(const Type &algorithm, const glm::vec2 &start, const glm::vec2 &end, float time) {
	// Update time
	time = CalculateNewTime(algorithm, time);

	// Lerp
	return glm::vec2(Lerp(start.x, end.x, time), Lerp(start.y, end.y, time));
}

glm::vec3 TweenCurve::Calculate(const Type &algorithm, const glm::vec3 &start, const glm::vec3 &end, float time) {
	// Update time
	time = CalculateNewTime(algorithm, time);

	// Lerp
	return glm::vec3(Lerp(start.r, end.r, time), Lerp(start.g, end.g, time), Lerp(start.b, end.b, time));
}

glm::vec4 TweenCurve::Calculate(const Type &algorithm, const glm::vec4 &start, const glm::vec4 &end, float time) {
	// Update time
	time = CalculateNewTime(algorithm, time);

	// Lerp
	return glm::vec4(Lerp(start.r, end.r, time), Lerp(start.g, end.g, time), Lerp(start.b, end.b, time), Lerp(start.a, end.a, time));
}

TweenCurve::TweenCurve(const TweenCurve &other)
	: startingValue(other.startingValue) {
	keyFrames.reserve(other.keyFrames.size());
	for(const KeyFrame &key : other.keyFrames) {
		keyFrames.emplace_back(key);
	}
}

bool TweenCurve::Initialize() {
	return true;
}

float TweenCurve::Calculate(float time) const {
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

bool TweenCurve::Read(Stream &stream) {
	// Read the starting value
	stream.Read(START, startingValue);

	// Go through the array of keyframes
	stream.ReadArray(KEYFRAMES, [this, &stream] () {
		KeyFrame key;

		// First, read the type of the keyframe
		int typeOrdinal = static_cast<int>(Type::Linear);
		stream.Read(TYPE, typeOrdinal);

		LOG_ASSERT(typeOrdinal < static_cast<int>(Type::Max), "TweenCurve::Read: unknown type read");
		key.type = static_cast<Type>(typeOrdinal);

		// Read the rest
		stream.Read(TIME, key.time);
		stream.Read(VALUE, key.value);

		// Add to the list
		keyFrames.emplace_back(key);
	});
	return true;
}

const std::string_view &TweenCurve::NameClass() const {
	static constexpr std::string_view className = NAMEOF(RassEngine::Components::HealthComponent);
	return className;
}

std::unique_ptr<TweenCurve> TweenCurve::Clone() const {
	return std::make_unique<TweenCurve>(*this);
}

}	// namespace
