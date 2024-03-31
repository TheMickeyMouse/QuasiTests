#version 330 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 color;

out vec4 gColor;

uniform mat4 u_projection, u_view;

void main() {
    gl_Position = u_projection * u_view * position;
    gColor = color;
}