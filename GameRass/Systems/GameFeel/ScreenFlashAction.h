#pragma once
#include "IGameFeelAction.h"
#include <glm/vec3.hpp>
#include <TweenCurve.h>

namespace RassGame::Systems {
class ScreenFlashAction : public IGameFeelAction {
public:
	bool Read(RassEngine::Stream &stream) override;
	void Execute(const FeelContext &ctx) override;
private:
	glm::vec3 color{1.0f, 1.0f, 1.0f};
	RassEngine::TweenCurve curve;
};
}
