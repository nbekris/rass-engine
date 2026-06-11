#version 330 core
in vec2 vUV;
out vec4 FragColor;

uniform sampler2D uScene;
uniform float uThreshold;   // e.g. 0.75
uniform float uKnee;        // soft transition width, e.g. 0.1

void main() {
    vec3 c = texture(uScene, vUV).rgb;

    // Perceptual brightness (luma). Alternatively: max(c.r, max(c.g, c.b))
    //float luma = dot(c, vec3(0.2126, 0.7152, 0.0722));
		float brightness = max(c.r, max(c.g, c.b));
    // ---- Soft-knee threshold (avoids hard popping at the cutoff) ----
    // Below (threshold - knee): 0
    // Above (threshold + knee): full
    // In between: smooth quadratic ramp
    float soft = brightness - uThreshold + uKnee;
    soft = clamp(soft, 0.0, 2.0 * uKnee);
    soft = soft * soft / (4.0 * uKnee + 0.0001);
    float contribution = max(soft, brightness - uThreshold);
    contribution /= max(brightness, 0.0001);
    FragColor = vec4(c * contribution, 1.0);
}
