#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in int rtype;

out vec4 v_color;
out vec2 v_TexCoord;
flat out int v_rtype;

uniform mat4 u_projection;
uniform mat4 u_view;

void main() {
    gl_Position = u_projection * u_view * position;
    v_rtype = rtype;
    v_TexCoord = texCoord;
    v_color = color;
}