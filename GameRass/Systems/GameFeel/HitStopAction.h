#pragma once
#include "IGameFeelAction.h"

namespace RassGame::Systems {
class HitStopAction : public IGameFeelAction {
public:
	bool Read(RassEngine::Stream &stream) override;
	void Execute(const FeelContext &ctx) override;
private:
	float duration{0.08f};    // real seconds to hold (unscaled)
	float timeScale{0.0f};    // scale during the freeze (0 = full stop)
};
}
