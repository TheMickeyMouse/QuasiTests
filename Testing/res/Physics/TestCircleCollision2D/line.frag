#version 330 core

layout (location = 0) out vec4 glColor;

flat in int colorID;

void main() {
    glColor = colorID > 3 ? vec4(0.0, 0.0, 1.0, 1.0) : vec4(1.0, 0.0, 0.0, 1.0);
}
