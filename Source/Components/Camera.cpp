// File Name:    Camera.cpp
// Author(s):    main Niko Bekris, secondary Boyuan Hu, Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Camera component defining the view and projection.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "Camera.h"

#include "Components/Transform.h"
#include "Entity.h"

namespace RassEngine::Components {
Camera::Camera() : Cloneable<Component, Camera>{}
{
}

Camera::Camera(const Camera &other)
: Cloneable<Component, Camera>{other}
, cameraSystem{other.cameraSystem}
, position{other.position}
, offset{other.offset}
, aimOffset{other.aimOffset}
, targetAimOffset{other.targetAimOffset}
, followSpeed{other.followSpeed}
, zoom{other.zoom}
, zoomLerpSpeed{other.zoomLerpSpeed}
, aimOffsetLerpSpeed{other.aimOffsetLerpSpeed}
, targetZoom{other.targetZoom}
, basicZoom{other.basicZoom}
, cameraWindowSize{other.cameraWindowSize}
, cameraWindowOffset{other.cameraWindowOffset}
, followMode{other.followMode}
, cameraWindowOffsetLerpSpeed{other.cameraWindowOffsetLerpSpeed}
, windowOffsetRange{other.windowOffsetRange}
{}

Camera::~Camera() {
	if(cameraSystem) {
		if(cameraSystem->GetActiveCamera() == this) {
			cameraSystem->UnregisterCamera(this);
		}
	}
}

bool Camera::Initialize() {
	cameraSystem = RassEngine::Systems::ICameraSystem::Get();

	cameraSystem->RegisterCamera(this);
	return true;
}

const std::string_view &Camera::NameClass() const {
	static constexpr std::string_view className = NAMEOF(Camera);
	return className;
}

bool Camera::Read(Stream &stream) {
	if(!Component::Read(stream)) {
		return false;
	}
	stream.ReadVec3("Position", position);
	stream.ReadVec3("Offset", offset);
	stream.ReadVec3("Zoom", zoom);
	targetZoom = zoom;
	basicZoom = zoom;
	//stream.ReadVec3("TargetZoom", targetZoom);
	stream.Read("ZoomLerpSpeed", zoomLerpSpeed);
	stream.Read("FollowSpeed", followSpeed);
	stream.ReadVec3("CameraWindowSize", cameraWindowSize);
	stream.ReadVec3("CameraWindowOffset", cameraWindowOffset);
	std::string followModeStr;
	if(stream.Read("FollowMode", followModeStr)) {
		if(followModeStr == "Lerp") {
			followMode = CameraFollowMode::Lerp;
		} else if(followModeStr == "Push") {
			followMode = CameraFollowMode::Push;
		}
	}
	stream.Read("CameraWindowOffsetLerpSpeed", cameraWindowOffsetLerpSpeed);
	stream.Read("WindowOffsetRange", windowOffsetRange);
	return true;
}
}
