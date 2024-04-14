#version 330 core

layout (location = 0) out vec4 glColor;

flat in int colorID;
uniform int blueThreshold;

void main() {
    glColor = colorID < blueThreshold ? vec4(0.0, 0.0, 1.0, 1.0) : vec4(1.0, 0.0, 0.0, 1.0);
}
