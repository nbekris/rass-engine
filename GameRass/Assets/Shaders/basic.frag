#version 330 core
in vec3 vertexColor;
in vec2 TexCoord;
in vec2 vModelPos;

out vec4 FragColor;

uniform vec3 uColorTint;
uniform float uAlpha;
uniform vec2 uTiling;
uniform vec2 uOffset;
uniform int uIsTextMode;
uniform sampler2D diffuseTex;

// Destructible tilemap
uniform int       uIsTileMap;
uniform usampler1D uTileStateMap;  // GL_R8UI 1D texture: 255=alive, 0=destroyed
uniform float     uTileSize;
uniform int       uMapCols;

void main() {
    // Tilemap: discard fragments whose tile has been destroyed
    if (uIsTileMap == 1) {
        // BuildStaticGrid sets x0 = col*tileSize, y0 = row*tileSize
        // so model-space XY directly encodes tile position
        int col = int(vModelPos.x / uTileSize);
        int row = int(vModelPos.y / uTileSize);
        int idx = row * uMapCols + col;
        uint alive = texelFetch(uTileStateMap, idx, 0).r;
        if (alive == 0u) {
            discard;
        }
    }

    vec2 finalUV;
    if (uIsTextMode == 1) {
        finalUV = TexCoord;
    } else {
        finalUV = TexCoord * uTiling + uOffset;
    }

    vec4 texColor = texture(diffuseTex, finalUV);

    if (texColor.a < 0.01) {
        discard;
    }

    FragColor = texColor * vec4(uColorTint, uAlpha);
}
