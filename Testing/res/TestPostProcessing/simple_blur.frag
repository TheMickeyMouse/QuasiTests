#version 330

layout (location = 0) out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform vec2 blurOff;

void main() {
    vec2 off = blurOff / vec2(textureSize(screenTexture, 0));
    vec3 total = vec3(0.0);
    for (int i = -4; i < 5; ++i) {
        float multiplier = float(1 << ((i & 1) + 2 * int(i == 0))) / 16.0;
        vec2 offset = off * vec2(float((i + 4) % 3) - 1.0, float((i + 4) / 3) - 1.0);
        total += multiplier * texture(screenTexture, TexCoords + offset).rgb;
    }
    FragColor = vec4(total, 1.0);
}