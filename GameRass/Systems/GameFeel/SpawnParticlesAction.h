#pragma once
#include <string>
#include <glm/vec3.hpp>
#include "IGameFeelAction.h"

namespace RassGame::Systems {
class SpawnParticlesAction : public IGameFeelAction {
public:
	bool Read(RassEngine::Stream &stream) override;
	void Execute(const FeelContext &ctx) override;
private:
	std::string prefabPath{};
	glm::vec3 offset{0.0f, 0.0f, 0.0f};
};
}
