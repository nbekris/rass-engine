#pragma once
#include "IGameFeelAction.h"
#include <Components/Camera.h>   // CameraShakeParams / CameraShakeEase

namespace RassGame::Systems {
class ScreenShakeAction : public IGameFeelAction {
public:
	bool Read(RassEngine::Stream &stream) override;
	void Execute(const FeelContext &ctx) override;
private:
	RassEngine::Components::CameraShakeParams params;
};
}
