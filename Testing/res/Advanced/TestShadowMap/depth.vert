#version 330 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec3 normal;

uniform mat4 u_projection, u_view;

void main() {
    gl_Position = u_projection * u_view * position;
}
