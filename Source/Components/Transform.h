// File Name:    Transform.h
// Author(s):    main Taro Omiya, secondary Boyuan Hu, Steven Yacoub
// Course:       GAM541
// Project:      RASS
// Purpose:      Component storing an entity position, rotation, and scale.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <glm/glm.hpp>
#include <string_view>
#include <unordered_set>

#include "Component.h"
#include "Cloneable.h"

// forward declaration
namespace RassEngine {
class Stream;
}

namespace RassEngine::Components {

class Transform : public Cloneable<Component, Transform> {
	using TypeChildrenList = std::unordered_set<Transform *>;
public:
	// Necessary to make this Transform's children iterable
	using TypeChildrenIterator = TypeChildrenList::iterator;

	using iterator_category = TypeChildrenIterator::iterator_category;
	using difference_type = TypeChildrenIterator::difference_type;
	using value_type = TypeChildrenIterator::value_type;
	using pointer = TypeChildrenIterator::pointer;
	using reference = TypeChildrenIterator::reference;

	inline TypeChildrenIterator begin() const {
		return children.begin();
	}

	inline TypeChildrenIterator end() const {
		return children.end();
	}

	inline TypeChildrenIterator begin() {
		return children.begin();
	}

	inline TypeChildrenIterator end() {
		return children.end();
	}
public:
	Transform();
	Transform(glm::vec3 position, glm::vec3 scale, float rotationRad);
	Transform(const Transform &other);
	~Transform() override;

	// Inherited via Object
	virtual bool Initialize() override;
	virtual const std::string_view &NameClass() const override;

	// Inherited via Component
	virtual bool Read(Stream& stream) override;

	// ============ Unity style API ============
	
	void SetLocalPosition(const glm::vec3 &position);
	inline glm::vec3 GetLocalPosition() const { return localPosition; }

	void SetLocalRotationRad(float angleRadians);
	inline void SetLocalRotationDeg(float angleDegrees) {
		SetLocalRotationRad(glm::radians(angleDegrees));
	}
	inline float GetLocalRotationRad() const {
		return localRotationRad;
	}
	inline float GetLocalRotationDeg() const {
		return glm::degrees(GetLocalRotationRad());
	}

	void SetLocalScale(const glm::vec3 &scale);
	inline glm::vec3 GetLocalScale() const { return localScale; }

	void SetPosition(const glm::vec3 &worldPosition);
	glm::vec3 GetPosition() const;
	glm::vec2 GetPosition2D() const;
	void SetRotationRad(float angleRadians);
	inline void SetRotationDeg(float angleDegrees) {
		SetRotationRad(glm::radians(angleDegrees));
	}
	float GetRotationRad() const;
	inline float GetRotationDeg() const {
		return glm::degrees(GetRotationRad());
	}

	glm::vec3 GetLossyScale() const;

	glm::mat4 getTransformMatrix() const;

	void SetParentTransform(Transform *parent, bool worldPositionStays = true);
	inline Transform* GetParentTransform() const { return parent; }

private:
	void MarkDirtyRecursive() const;

	// only stores local transform( to father node )
	glm::vec3 localPosition{0.0f, 0.0f, 0.0f};
	glm::vec3 localScale{1.0f, 1.0f, 1.0f};
	float localRotationRad{0.0f};

	//parent transform node pointer
	Transform *parent{nullptr};
	TypeChildrenList children{};

	// cached world matrix
	mutable glm::mat4 worldMatrix{1.0f};
	mutable glm::vec3 lossyGlobalScale{1.0f, 1.0f, 1.0f};
	mutable bool isDirty{true};
};

}
