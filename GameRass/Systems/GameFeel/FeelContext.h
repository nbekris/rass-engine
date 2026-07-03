// Purpose: Per-trigger runtime snapshot passed to actions at Execute time.
//          Values only — never hold Entity* (detached playback outlives the entity).
#pragma once
#include <glm/vec3.hpp>

namespace RassGame::Systems {
struct FeelContext {
	glm::vec3 position{0.0f, 0.0f, 0.0f};   // world position captured at Play() time
	float rotationRad{0.0f};
};
}
