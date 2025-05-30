#version 330 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec2 bcoord;
layout (location = 2) in vec4 color;

out vec4 vPosition;
out vec2 vBezCoord;
out vec4 vColor;

uniform mat4 u_projection, u_view;

void main() {
    gl_Position = u_projection * u_view * position;
    vPosition = position;
    vBezCoord = bcoord;
    vColor = color;
}
