// File Name:    Shader.h
// Author(s):    main Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Shader program loading and management.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <string_view>

// Forward declaration
namespace gl {
enum class GLenum : unsigned int;
}

namespace RassEngine::Graphics {

class Shader {
public:
	Shader();
	void AddShader(const std::string_view &fileName, const gl::GLenum shaderType);
	void LinkProgram();
	void UseShader();
	void UnuseShader();
	inline unsigned int GetProgramId() const {
		return programId;
	}

private:
	unsigned int programId;
};

}
