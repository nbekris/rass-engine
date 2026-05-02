#pragma once

#include <cmath>
#include <cstdlib>
#include <glm/vec3.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
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

	static glm::mat4 GetTransformMatrix(const glm::vec3 &position, float rotationRad, const glm::vec3 &scale) {
		static const glm::vec3 ROTATE_AXIS{0.0f, 0.0f, 1.0f};

		// calculate local matrix
		glm::mat4 toReturn(1.0f);
		toReturn = glm::translate(toReturn, position);
		toReturn = glm::rotate(toReturn, rotationRad, ROTATE_AXIS);
		toReturn = glm::scale(toReturn, scale);
		return toReturn;
	}
};

}
