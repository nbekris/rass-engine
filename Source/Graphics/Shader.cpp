// File Name:    Shader.cpp
// Author(s):    main Taro Omiya, secondary Niko Bekris, Eric Fleegal
// Course:       GAM541
// Project:      RASS
// Purpose:      Shader program loading and management.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "Graphics/Shader.h"

#include <cstdlib>
#include <memory>
#include <fstream>
#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>
#include <ios>
#include <string_view>

#include "Systems/Logging/ILoggingSystem.h"
#include "Utils.h"

using namespace gl;

namespace RassEngine::Graphics {

using CharBuffer = std::unique_ptr<char[]>;

CharBuffer ReadFile(const std::string_view &name) {
	std::ifstream f;
	const std::string fileName{name};
	f.open(fileName, std::ios_base::binary); // Open
	if(f.fail()) {
		LOG_ERROR("ERROR! Can't read/find shader file: {}\n", fileName);
		exit(-1);
	}
	f.seekg(0, std::ios_base::end);      // Position at end
	size_t length = static_cast<size_t>(f.tellg());              //   to get the length

	CharBuffer content = std::make_unique<char[]>(length + 1); // Create buffer of needed length
	f.seekg(0, std::ios_base::beg);     // Position at beginning
	f.read(content.get(), length);            //   to read complete file
	f.close();                           // Close

	content[length] = char(0);           // Finish with a NULL
	return content;
}

// Creates an empty shader program.
Shader::Shader() : programId{glCreateProgram()} {}

// Use a shader program
void Shader::UseShader() {
	glUseProgram(programId);
}

// Done using a shader program
void Shader::UnuseShader() {
	glUseProgram(0);
}

void Shader::AddShader(const std::string_view &fileName, GLenum type) {
	CharBuffer src = ReadFile(fileName);
	const char *psrc[1] = {src.get()};

	// Create a shader and attach, hand it the source, and compile it.
	int shader = glCreateShader(type);
	glAttachShader(programId, shader);
	glShaderSource(shader, 1, psrc, NULL);
	glCompileShader(shader);
	src.reset();

	// Get the compilation status
	int status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	// If compilation status is not OK, get and print the log message.
	if(status != 1) {
		int length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		CharBuffer buffer = std::make_unique<char[]>(length);
		glGetShaderInfoLog(shader, length, NULL, buffer.get());


		LOG_ERROR("========================================");
		LOG_ERROR("ERROR: Shader Compilation Failed");
		LOG_ERROR("File: {}", fileName);
		LOG_ERROR("Type: {}",
			type == GL_VERTEX_SHADER ? "Vertex Shader" :
			type == GL_FRAGMENT_SHADER ? "Fragment Shader" : "Unknown");
		LOG_ERROR("========================================");
		LOG_ERROR("{}", buffer.get());
		LOG_ERROR("========================================");
		exit(-1);
	} else {

		LOG_INFO("Shader compiled successfully: {}", fileName);
	}
}

void Shader::LinkProgram() {
	// Link program and check the status
	glLinkProgram(programId);
	int status;
	glGetProgramiv(programId, GL_LINK_STATUS, &status);

	// If link failed, get and print log
	if(status != 1) {
		int length;
		glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &length);
		CharBuffer buffer = std::make_unique<char[]>(length);
		glGetProgramInfoLog(programId, length, NULL, buffer.get());

		LOG_ERROR("========================================");
		LOG_ERROR("ERROR: Shader Program Linking Failed");
		LOG_ERROR("Program ID: {}", programId);
		LOG_ERROR("========================================");
		LOG_ERROR("{}", buffer.get());
		LOG_ERROR("========================================");
		exit(-1);
	} else {

		LOG_INFO("Shader program {} linked successfully.", programId);
	}

}

}
