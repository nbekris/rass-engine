#include <Precompiled.h>            // match sibling GameFeelFactory.cpp
#include "ScreenShakeAction.h"

#include <string>
#include <Stream.h>
#include <Components/Camera.h>
#include <Systems/Camera/ICameraSystem.h>

using namespace RassEngine;
using namespace RassEngine::Systems;

namespace RassGame::Systems {
bool ScreenShakeAction::Read(Stream &stream) {
	stream.Read("Duration", params.shakeDuration);
	stream.Read("VibrationSpeed", params.vibrationSpeed);
	stream.ReadVec3("MaxTranslation", params.maxTranslation);
	stream.Read("MaxZRotation", params.maxZRotation);

	std::string ease;
	if(stream.Read("Ease", ease)) {
		if(ease == "Linear")         params.easeType = Components::CameraShakeEase::Linear;
		else if(ease == "EaseIn")    params.easeType = Components::CameraShakeEase::EaseIn;
		else if(ease == "EaseOut")   params.easeType = Components::CameraShakeEase::EaseOut;
		else if(ease == "EaseInOut") params.easeType = Components::CameraShakeEase::EaseInOut;
		else if(ease == "EaseOutIn") params.easeType = Components::CameraShakeEase::EaseOutIn;
	}
	return true;
}

void ScreenShakeAction::Execute(const FeelContext &) {
	if(auto *cam = ICameraSystem::Get()) {   // interface already exposes ShakeCamera
		cam->ShakeCamera(params);
	}
}
}
