// File Name:    Texture.h
// Author(s):    main Boyuan Hu, secondary Taro Omiya, Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Texture resource loading and binding.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <string>

///////////////////////////////////////////////////////////////////////
// A slight encapsulation of an OpenGL texture. This contains a method
// to read an image file into a texture, and methods to bind a texture
// to a shader for use, and unbind when done.
////////////////////////////////////////////////////////////////////////

// This class reads an image from a file, stores it on the graphics
// card as a texture, and stores the (small integer) texture id which
// identifies it.  It also supplies two methods for binding and
// unbinding the texture to/from a shader.

namespace RassEngine::Graphics {

class Texture {
public:
	Texture();
	Texture(const std::string &path, bool useLinear = false);

	void BindTexture(const int unit, const int programId, const std::string &name);
	void UnbindTexture(const int unit);

	inline unsigned int GetTextureID() const { return textureId; }
	inline int GetWidth() const { return width; }
	inline int GetHeight() const { return height; }

private:
	unsigned int textureId{0};
	int width{0}, height{0}, depth{0};
	unsigned char *image{nullptr};
};

}
