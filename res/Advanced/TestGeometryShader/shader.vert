#version 330 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec3 normal;

out vec3 vPosition;
out vec3 vNormal;
out vec4 vColor;

uniform mat4 u_projection, u_view;

void main() {
    gl_Position = u_projection * u_view * position;
    vPosition = position.xyz;
    vNormal = normal;
    vColor = color;
}