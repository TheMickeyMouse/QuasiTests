#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in int textureID;

out vec2 v_TexCoord;
out vec4 v_color;
flat out int v_index;

uniform mat4 u_projection;
uniform mat4 u_view;

void main() {
    gl_Position = u_projection * u_view * position;
    v_color = color;
    v_TexCoord = texCoord;
    v_index = textureID;
}