#version 330 core
in vec2 vUV;
out vec4 FragColor;

uniform sampler2D uScene;
uniform sampler2D uBloom;
uniform float uBloomIntensity;
uniform int   uHdr;

void main() {
    vec3 scene = texture(uScene, vUV).rgb;
    vec3 bloom = texture(uBloom, vUV).rgb;

    vec3 color = scene + bloom * uBloomIntensity;

//    if (uHdr == 1) {
//        // Reinhard tonemap: smoothly compress HDR range into [0,1].
//        color = color / (color + vec3(1.0));
//    }
    // LDR path unchanged: values just clamp at output as before.

    FragColor = vec4(color, 1.0);
}
