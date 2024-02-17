#version 330

layout (location = 0) out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform vec2 detectOff;

void main() {
    vec2 off = detectOff / vec2(textureSize(screenTexture, 0));
    vec3 total = vec3(0.0);
    for (int i = 0; i < 9; ++i) {
        vec2 offset = off * vec2(float(i % 3) - 1.0, float(i / 3) - 1.0);
        total += (i == 4 ? -8.0 : 1.0) * texture(screenTexture, TexCoords + offset).rgb;
    }
    FragColor = vec4(total, 1.0);
}
