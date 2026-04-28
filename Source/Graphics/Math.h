#pragma once

#include <cmath>
#include <cstdlib>
#include <glm/vec3.hpp>
#include <limits>

namespace RassEngine::Graphics {

struct Math {
	static glm::vec3 FromAngleRad(float angleRad) {
		return glm::vec3(std::cosf(angleRad), std::sinf(angleRad), 0);
	}

	static float ToAngleRad(const glm::vec3 &vector) {
		// Make sure vector isn't (0, 0)
		if((std::abs(vector.x) < std::numeric_limits<float>::epsilon())
			&& (std::abs(vector.y) < std::numeric_limits<float>::epsilon())) {
			// If it is, just return 0
			return 0;
		}

		return std::atan2f(vector.y, vector.x);
	}
};

}
