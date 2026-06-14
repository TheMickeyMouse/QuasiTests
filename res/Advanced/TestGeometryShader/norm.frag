#version 330 core

layout (location = 0) out vec4 glColor;

in vec4 vColor;

void main() {
    glColor = vColor;
}