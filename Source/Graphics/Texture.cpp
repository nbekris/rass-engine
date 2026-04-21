// File Name:    Texture.cpp
// Author(s):    main Taro Omiya, secondary Boyuan Hu, Eric Fleegal, Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Texture resource loading and binding.
//
// Copyright © 2026 DigiPen (USA) Corporation.

///////////////////////////////////////////////////////////////////////
// A slight encapsulation of an OpenGL texture. This contains a method
// to read an image file into a texture, and methods to bind a texture
// to a shader for use, and unbind when done.
////////////////////////////////////////////////////////////////////////
#include "Precompiled.h"
#include "Graphics/texture.h"

#include <cstdlib>
#include <fstream>
#include <math.h>

#pragma warning(push)
#pragma warning(disable: 4251)
#include <glbinding/gl/gl.h>
#include <glbinding/Binding.h>
#pragma warning(pop)
using namespace gl;

#define GLM_FORCE_CTOR_INIT
#define GLM_FORCE_RADIANS
#define GLM_SWIZZLE
#include <glm/glm.hpp>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include <stb_image.h>

#include "Systems/Logging/ILoggingSystem.h"
#include "Utils.h"

namespace RassEngine::Graphics {

Texture::Texture() : textureId{0}, width{0}, height{0}, depth{0} {}

Texture::Texture(const std::string &path, bool useLinear) : textureId(0) {
	stbi_set_flip_vertically_on_load(true);
	image = stbi_load(path.c_str(), &width, &height, &depth, 4);
	LOG_INFO("{} {} {} {}", depth, width, height, path.c_str());
	if(!image) {
		LOG_ERROR("Read error on file {}:\n  {}\n\n", path.c_str(), stbi_failure_reason());
		exit(-1);
	}

	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, (GLint)GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 10);
	glGenerateMipmap(GL_TEXTURE_2D);

	if(useLinear) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (int)GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (int)GL_LINEAR_MIPMAP_LINEAR);
	} else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (int)GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (int)GL_NEAREST);
	}
	if(path.contains("Background")) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(image);
}

// Make a texture availabe to a shader program.  The unit parameter is
// a small integer specifying which texture unit should load the
// texture.  The name parameter is the sampler2d in the shader program
// which will provide access to the texture.
void Texture::BindTexture(const int unit, const int programId, const std::string &name) {
	glActiveTexture((gl::GLenum)((int)GL_TEXTURE0 + unit));
	glBindTexture(GL_TEXTURE_2D, textureId);
	int loc = glGetUniformLocation(programId, name.c_str());
	glUniform1i(loc, unit);
}

// Unbind a texture from a texture unit whne no longer needed.
void Texture::UnbindTexture(const int unit) {
	glActiveTexture((gl::GLenum)((int)GL_TEXTURE0 + unit));
	glBindTexture(GL_TEXTURE_2D, 0);
}

}
