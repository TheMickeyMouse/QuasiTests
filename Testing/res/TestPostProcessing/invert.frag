#version 330

layout (location = 0) out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

void main() {
    FragColor = vec4(1.0 - texture(screenTexture, TexCoords).rgb, 1.0);
}