// File Name:    Transform.cpp
// Author(s):    main Boyuan Hu, secondary Taro Omiya, Steven Yacoub, Niko Bekris, Eric Fleegal
// Course:       GAM541
// Project:      RASS
// Purpose:      Component storing an entity position, rotation, and scale.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "Components/Transform.h"

#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Cloneable.h"
#include "Component.h"
#include "Stream.h"
#include "Utils.h"

namespace RassEngine::Components {

Transform::Transform()
	: Cloneable<Component, Transform>{}
	, localPosition{0.0f, 0.0f, 0.0f}, localScale{1.0f, 1.0f, 1.0f}, localRotationRad{0.0f} {}

Transform::Transform(glm::vec3 position, glm::vec3 scale, float rotationRad)
	: Cloneable<Component, Transform>{}
	, localPosition{position}, localScale{scale}, localRotationRad{rotationRad} {}

Transform::Transform(const Transform &other)
	: Cloneable<Component, Transform>{other}
	, localPosition{other.localPosition}, localScale{other.localScale}, localRotationRad{other.localRotationRad}
	, parent{nullptr}, children{} {}
// deep copy, do not copy parent pointer

Transform::~Transform() {
	// Indicate the parent to remove this from their children list
	SetParentTransform(nullptr, false);

	// Indicate to all children they do not have a parent, anymore
	for(Transform *child : children) {
		if(child != nullptr) {
			child->parent = nullptr;
		}
	}

	// Clear the children list
	children.clear();
}

void Transform::SetLocalPosition(const glm::vec3 &position) {
	localPosition = position;
	MarkDirtyRecursive();
}

void Transform::SetLocalRotationRad(float angleRadians) {
	localRotationRad = angleRadians;
	MarkDirtyRecursive();
}

void Transform::SetLocalScale(const glm::vec3 &scale) {
	localScale = scale;
	MarkDirtyRecursive();
}

//get world position
glm::vec3 Transform::GetPosition() const {
	glm::mat4 worldMat = getTransformMatrix();
	return glm::vec3(worldMat[3]); // get translation part from world matrix
}

glm::vec2 Transform::GetPosition2D() const {
	glm::vec3 worldPos = GetPosition();
	return glm::vec2(worldPos.x, worldPos.y);
}

void Transform::SetPosition(const glm::vec3 &worldPosition) {
	if(parent == nullptr) {
		// if no parent, local position = world position
		localPosition = worldPosition;
	} else {
		// with parent, calculate local position
		// worldPosition = parentMatrix * localPosition
		// => localPosition = inverse(parentMatrix) * worldPosition
		glm::mat4 parentMat = parent->getTransformMatrix();
		glm::mat4 invParentMat = glm::inverse(parentMat);
		glm::vec4 localPos4 = invParentMat * glm::vec4(worldPosition, 1.0f);
		localPosition = glm::vec3(localPos4);
	}
	MarkDirtyRecursive();
}

float Transform::GetRotationRad() const {
	glm::mat4 worldMat = getTransformMatrix();
	// get angle from world matrix
	return glm::atan(worldMat[0][1], worldMat[0][0]);
}

void Transform::SetRotationRad(float angleRadians) {
	if(parent == nullptr) {
		localRotationRad = angleRadians;
	} else {
		// minus parent rotation
		float parentRotation = parent->GetRotationRad();
		localRotationRad = angleRadians - parentRotation;
	}
	MarkDirtyRecursive();
}

glm::vec3 Transform::GetLossyScale() const {
	// only approximate lossy scale by extracting scale from world matrix columns
	getTransformMatrix();
	return lossyGlobalScale;
}

glm::mat4 Transform::getTransformMatrix() const {
	static const glm::vec3 ROTATE_AXIS{0.0f, 0.0f, 1.0f};
	if(isDirty) {
		// calculate local matrix
		worldMatrix = glm::mat4(1.0f);
		worldMatrix = glm::translate(worldMatrix, localPosition);
		worldMatrix = glm::rotate(worldMatrix, localRotationRad, ROTATE_AXIS);
		worldMatrix = glm::scale(worldMatrix, localScale);

		// Calculate global scale
		lossyGlobalScale = localScale;

		//multiply parent matrix if exists
		if(parent != nullptr) {
			worldMatrix = parent->getTransformMatrix() * worldMatrix;
			lossyGlobalScale *= parent->GetLossyScale();
		}

		isDirty = false;
	}

	return worldMatrix;
}

void Transform::SetParentTransform(Transform *newParent, bool worldPositionStays) {
	// Make sure we're not creating circular references
	if(newParent == this || newParent == this->parent) {
		return;
	}

	if(newParent != nullptr) {
		Transform *ancestor = newParent;
		while(ancestor != nullptr) {
			if(ancestor == this) {
				//if we found 'this' in the ancestor chain, abort
				return;
			}
			ancestor = ancestor->parent;
		}
	}

	// Clean-up the old parent
	if(parent != nullptr) {
		// Notify the parent to remove this transform
		parent->children.erase(this);
	}

	// Check if we're ignoring the world-position
	if(!worldPositionStays) {
		// directly set new parent
		parent = newParent;

		if (parent != nullptr) {
			// Update the parent's children set
			parent->children.emplace(this);
		}

		// Halt after marking this transform dirty
		MarkDirtyRecursive();
		return;
	}

	// keep world position, rotation and scale unchanged
	glm::vec3 worldPos = GetPosition();
	float worldRot = GetRotationRad();
	glm::vec3 worldScale = GetLossyScale();

	// directly set new parent
	parent = newParent;

	// Update the parent's children set
	if(parent != nullptr) {
		parent->children.emplace(this);
	}

	//calculate new local position and rotation
	SetPosition(worldPos);
	SetRotationRad(worldRot);

	// calculate new local scale
	if(newParent != nullptr) {
		glm::vec3 parentScale = newParent->GetLossyScale();
		//avoid division by zero
		localScale.x = (std::abs(parentScale.x) > std::numeric_limits<float>::epsilon()) ? worldScale.x / parentScale.x : worldScale.x;
		localScale.y = (std::abs(parentScale.y) > std::numeric_limits<float>::epsilon()) ? worldScale.y / parentScale.y : worldScale.y;
		localScale.z = (std::abs(parentScale.z) > std::numeric_limits<float>::epsilon()) ? worldScale.z / parentScale.z : worldScale.z;
	} else {
		// if no parent, local scale = world scale
		localScale = worldScale;
	}
	MarkDirtyRecursive();
}

void Transform::MarkDirtyRecursive() const {
	isDirty = true;
	for(Transform *child : children) {
		child->MarkDirtyRecursive();
	}
}

bool Transform::Initialize() {
	return true;
}

const std::string_view &Transform::NameClass() const {
	static constexpr std::string_view className = NAMEOF(RassEngine::Components::Transform);
	return className;
}

bool Transform::Read(Stream& stream) {
	if(!Component::Read(stream)) {
		return false;
	}
	// Read the position (which could be Translate)
	if(stream.Contains("Position")) {
		stream.ReadVec3("Position", localPosition);
	} else if(stream.Contains("Translation")) {
		stream.ReadVec3("Translation", localPosition);
	}

	// Read the scale
	stream.ReadVec3("Scale", localScale);

	// Read the rotation as in degrees
	float angleDegrees;
	if(stream.Read("Rotation", angleDegrees)) {
		// If one is provided, convert it to radians
		SetLocalRotationDeg(angleDegrees);
	}

	return true;
}

}
