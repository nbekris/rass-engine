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
uniform int uIsMultiply;
uniform sampler2D diffuseTex;

// Destructible tilemap
uniform int       uIsTileMap;
uniform usampler1D uTileStateMap;  // GL_R8UI 1D: 0 = whole, 255 = fully dissolved
uniform float     uTileSize;
uniform int       uMapCols;
const float bayer4x4[16] = float[](
     0.0/16.0,  8.0/16.0,  2.0/16.0, 10.0/16.0,
    12.0/16.0,  4.0/16.0, 14.0/16.0,  6.0/16.0,
     3.0/16.0, 11.0/16.0,  1.0/16.0,  9.0/16.0,
    15.0/16.0,  7.0/16.0, 13.0/16.0,  5.0/16.0
);

void main() {
    float dissolveGlow = 0.0;   // accumulate now, apply at the very end

    if (uIsTileMap == 1) {
        int col = int(vModelPos.x / uTileSize);
        int row = int(vModelPos.y / uTileSize);
        int idx = row * uMapCols + col;
        float dissolve = float(texelFetch(uTileStateMap, idx, 0).r) / 255.0;

        if (dissolve > 0.0) {
            float density = 8.0 / uTileSize;
            ivec2 dc = ivec2(floor(vModelPos.xy * density)) & 3;
            float threshold = bayer4x4[dc.y * 4 + dc.x];

            if (dissolve > threshold) {
                discard;
            } else {
                float edge = threshold - dissolve;   // near zero = almost disappear
                if (edge < 0.08) {
                    dissolveGlow = 1.0 - edge / 0.08;
                }
            }
        }
    }

    vec2 finalUV = (uIsTextMode == 1) ? TexCoord : (TexCoord * uTiling + uOffset);

    vec4 texColor = texture(diffuseTex, finalUV);
    if (texColor.a < 0.01) {
        discard;
    }

    if (uIsMultiply == 1) {
        vec3  tinted = texColor.rgb * uColorTint;
        float a      = texColor.a   * uAlpha;
        FragColor = vec4(mix(vec3(1.0), tinted, a), 1.0);
    } else {
        FragColor = texColor * vec4(uColorTint, uAlpha);
    }

    // Emissive edge (>1 so it feeds bloom). Applied AFTER base color so it survives.
    FragColor.rgb += vec3(2.5, 1.2, 0.3) * dissolveGlow;
}
