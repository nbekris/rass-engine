#include "Precompiled.h"
#include <glbinding/gl/gl.h>
#include <glbinding/Binding.h>
using namespace gl;

#include "Graphics/fbo.h"
#include "Systems/Logging/ILoggingSystem.h"
namespace RassEngine::Graphics {
void FBO::CreateFBO(int w, int h) {
	if(w <= 0 || h <= 0) {
		LOG_DEBUG("Invalid FBO size: {}x{}", w, h);
		return;
	}
	width = w;
	height = h;

	glGenFramebuffers(1, &fboID);
	glBindFramebuffer(GL_FRAMEBUFFER, fboID);

	glGenRenderbuffers(1, &depthRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, depthRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_RENDERBUFFER, depthRBO);

	// HDR uses RGBA16F (values can exceed 1.0); LDR keeps RGBA8.
	// 16F is guaranteed linear-filterable in core 3.3; 32F is not.
	const GLenum internalFmt = hdr ? GL_RGBA16F : GL_RGBA8;
	const GLenum pixelType = hdr ? GL_FLOAT : GL_UNSIGNED_BYTE;

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(internalFmt),
		width, height, 0, GL_RGBA, pixelType, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(GL_CLAMP_TO_EDGE));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(GL_CLAMP_TO_EDGE));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(GL_LINEAR));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(GL_LINEAR));

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, textureID, 0);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(status != GL_FRAMEBUFFER_COMPLETE) {
		LOG_ERROR("FBO incomplete: status = {}", static_cast<unsigned int>(status));
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FBO::Resize(int w, int h) {
	if(w == width && h == height) {
		return;  // no change
	}
	if(w <= 0 || h <= 0) {
		LOG_DEBUG("Invalid FBO size: {}x{}", w, h);
		return;
	}
	Destroy();
	CreateFBO(w, h);
}

void FBO::Destroy() {
	if(textureID) {
		glDeleteTextures(1, &textureID);      textureID = 0;
	}
	if(depthRBO) {
		glDeleteRenderbuffers(1, &depthRBO);  depthRBO = 0;
	}
	if(fboID) {
		glDeleteFramebuffers(1, &fboID);      fboID = 0;
	}
}

void FBO::BindFBO() {
	glBindFramebuffer(GL_FRAMEBUFFER, fboID);
}
void FBO::UnbindFBO() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FBO::BindTexture(int unit, int programId, const std::string &name) {
	glActiveTexture(static_cast<GLenum>(static_cast<int>(GL_TEXTURE0) + unit));
	glBindTexture(GL_TEXTURE_2D, textureID);
	int loc = glGetUniformLocation(programId, name.c_str());
	glUniform1i(loc, unit);
}

void FBO::UnbindTexture(int unit) {
	glActiveTexture(static_cast<GLenum>(static_cast<int>(GL_TEXTURE0) + unit));
	glBindTexture(GL_TEXTURE_2D, 0);
}

void FBO::SetHdr(bool enable) {
	if(enable == hdr) {
		return;   // no change
	}
	hdr = enable;
	if(fboID) {          // already created → rebuild at same size with new format
		const int w = width, h = height;
		Destroy();
		CreateFBO(w, h);
	}
	// if not yet created, the next Resize/CreateFBO will pick up the new flag
}
}
