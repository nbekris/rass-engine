#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 vertexColor;
out vec2 TexCoord;
out vec2 vModelPos;  // model-space XY for tilemap tile-index lookup

uniform mat4 projection;
uniform mat4 modelTransform;
uniform mat4 view;

// Text mode
uniform int uIsTextMode;
uniform int uCharCount;
uniform vec2 uCharUVOffsets[128];
uniform vec2 uTiling;

void main() {
    gl_Position = projection * view * modelTransform * vec4(aPos, 1.0);
    vertexColor = aColor;
    // Pass raw model-space XY; fragment shader computes col = int(x/tileSize), row = int(y/tileSize)
    vModelPos = aPos.xy;

    if (uIsTextMode == 1) {
        int charSlot = gl_VertexID / 6;
        int vertexInQuad = gl_VertexID % 6;

        if (charSlot >= uCharCount) {
            gl_Position = vec4(-9999.0, -9999.0, 0.0, 1.0);
            TexCoord = vec2(0.0);
            return;
        }

        vec2 offset = uCharUVOffsets[charSlot];

        vec2 localUV[6] = vec2[6](
            vec2(0.0, 1.0),
            vec2(0.0, 0.0),
            vec2(1.0, 0.0),
            vec2(0.0, 1.0),
            vec2(1.0, 0.0),
            vec2(1.0, 1.0)
        );

        TexCoord = localUV[vertexInQuad] * uTiling + offset;
    }
    else {
        TexCoord = aTexCoord;
    }
}
