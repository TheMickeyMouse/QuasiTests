#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;

out vec4 v_color;
out float v_alpha;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform float u_alpha;

void main() {
    gl_Position = u_projection * u_view * position;
    v_color = color;
    v_alpha = u_alpha;
}