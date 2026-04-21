// File Name:    Camera.h
// Author(s):    main Niko Bekris, secondary Boyuan Hu, Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Camera component defining the view and projection.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include "Cloneable.h"
#include "Component.h"
#include "Stream.h"
#include "Entity.h"

#include "Systems/Camera/ICameraSystem.h"

namespace RassEngine::Components {
class Camera : public Cloneable<Component, Camera> {
public:
	enum class CameraFollowMode {
		Lerp,
		Push //Character push the camera window like push a box.
	};
	Camera();
	Camera(const Camera &other);
	virtual ~Camera() override;
	// Inherited via Object
	virtual bool Initialize() override;
	virtual const std::string_view &NameClass() const override;
	// Inherited via Component
	virtual bool Read(Stream &stream) override;

	inline Transform *GetTransform() const {
		return Parent()->GetTransform();
	}

public:
	glm::vec3 position{0.0f, 0.0f, 0.0f};
	glm::vec3 offset{0.0f, 0.0f, 0.0f};           // Base follow offset (from data file), affects window center
	glm::vec3 aimOffset{0.0f, 0.0f, 0.0f};        // Current smoothed aim look-ahead (rendering only)
	glm::vec3 targetAimOffset{0.0f, 0.0f, 0.0f};  // Desired aim look-ahead (written by ShootingController)
	glm::vec3 zoom{1.0f, 1.0f, 1.0f};
	float followSpeed = 0.5f;
	float zoomLerpSpeed = 1.0f;
	float aimOffsetLerpSpeed{5.0f};
	glm::vec3 targetZoom{1.0f, 1.0f, 1.0f};
	glm::vec3 basicZoom{1.0f, 1.0f, 1.0f};
	glm::vec3 cameraWindowSize{4.0f, 2.0f, 1.0f};
	glm::vec3 cameraWindowOffset{0.0f,0.0f, 0.0f};
	glm::vec3 targetCameraWindowOffset{0.0f, 0.0f, 0.0f};
	glm::vec3 basicCameraWindowOffset{0.0f, 0.0f, 0.0f};
	float cameraWindowOffsetLerpSpeed{2.0f};
	float windowOffsetRange{2.0f};
	CameraFollowMode followMode = CameraFollowMode::Push;
private:
	RassEngine::Systems::ICameraSystem *cameraSystem{nullptr};
};
}

