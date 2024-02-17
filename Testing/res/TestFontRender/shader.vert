#version 330

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in int isText;

out vec4 v_color;
out vec2 v_texCoord;
flat out int v_isText;

uniform mat4 u_projection;
uniform mat4 u_view;

void main() {
    gl_Position = u_projection * u_view * position;
    v_color = color;
    v_texCoord = texCoord;
    v_isText = isText;
}