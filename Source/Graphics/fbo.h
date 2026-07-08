#pragma once

#include <string>

namespace gl {
enum class GLenum : unsigned int;
}

namespace RassEngine::Graphics {

// Single-target color FBO for 2D forward rendering + post-processing.
class FBO {
public:
	unsigned int fboID = 0;
	unsigned int textureID = 0;
	unsigned int depthRBO = 0;
	int width = 0, height = 0;
	bool hdr = false;   // false = RGBA8 (LDR), true = RGBA16F (HDR)

	FBO() = default;
	~FBO() {
		Destroy();
	}

	FBO(const FBO &) = delete;
	FBO &operator=(const FBO &) = delete;

	FBO(FBO &&other) noexcept;
	FBO &operator=(FBO &&other) noexcept;

	void CreateFBO(int w, int h);
	void Resize(int w, int h);
	void SetHdr(bool enable);   // recreate with new internal format if changed
	void Destroy();

	void BindFBO();
	void UnbindFBO();
	void BindTexture(int unit, int programId, const std::string &name);
	void UnbindTexture(int unit);
};

}   // namespace RassEngine::Graphics
