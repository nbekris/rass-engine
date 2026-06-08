#pragma once
#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <initializer_list>
#include <memory>
#include <string_view>

#include "ICloneable.h"
#include "ISerializable.h"
#include "Object.h"

namespace RassEngine {
// Forward Declarations:
class Stream;

// Class Definition:
class TweenCurve : public Object, public ICloneable<TweenCurve>, public ISerializable<Stream> {
public:
	enum class Type : unsigned char {
		Linear = 0,
		EaseInCircle,
		EaseOutCircle,
		Max
	};

	struct KeyFrame {
		Type type{Type::Linear};
		float time{0.f};
		float value{0.f};
	};

	static float Calculate(const Type &algorithm, float startValue, float endValue, float time);
	static glm::vec2 Calculate(const Type &algorithm, const glm::vec2 &startValue, const glm::vec2 &endValue, float time);
	static glm::vec3 Calculate(const Type &algorithm, const glm::vec3 &startValue, const glm::vec3 &endValue, float time);
	static glm::vec4 Calculate(const Type &algorithm, const glm::vec4 &startValue, const glm::vec4 &endValue, float time);

	TweenCurve() = default;
	TweenCurve(const TweenCurve &other);
	virtual ~TweenCurve() = default;

	void Set(float startValue, std::initializer_list<KeyFrame> keyFrames);
	float Calculate(float time) const;
	float GetTotalDuration() const;
	float GetEndingValue() const;
	inline float GetStartingValue() const {
		return startingValue;
	}

	// Inherited via Object
	bool Initialize() override;
	const std::string_view &NameClass() const override;

	// Inherited via ICloneable
	std::unique_ptr<TweenCurve> Clone() const override;

	// Inherited via ISerializable
	bool Read(Stream &stream) override;

private:
	float startingValue{1.f};
	std::vector<KeyFrame> keyFrames;
};

}	// namespace
