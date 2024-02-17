#version 330 core

layout(location = 0) out vec4 color;

in vec4 v_color;
in float v_alpha;

void main() {
    color = v_color;
    color.a = v_alpha;
}