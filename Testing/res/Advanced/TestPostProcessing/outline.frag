#version 330 core

layout (location = 0) out vec4 glFragColor;

uniform vec4 outlineColor;

void main() {
    glFragColor = outlineColor;
}