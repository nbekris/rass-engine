#version 330 core
in vec2 vUV;
out vec4 FragColor;

uniform sampler2D uImage;
uniform vec2 uDirection;   // horizontal: (1/texW, 0)  vertical: (0, 1/texH)

// 9-tap Gaussian (center + 4 each side), normalized weights
const float weight[5] = float[](
    0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216
);

void main() {
    vec3 result = texture(uImage, vUV).rgb * weight[0];
    for(int i = 1; i < 5; ++i) {
        vec2 offset = uDirection * float(i);
        result += texture(uImage, vUV + offset).rgb * weight[i];
        result += texture(uImage, vUV - offset).rgb * weight[i];
    }
    FragColor = vec4(result, 1.0);
}