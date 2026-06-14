#version 330

layout (location = 0) in vec4 position;
layout (location = 1) in vec2 textureCoord;
layout (location = 2) in vec3 normal;

out vec3 vPosition;
out vec2 vTexCoord;
out vec3 vNormal;

uniform mat4 u_projection, u_view;

void main() {
    gl_Position = u_projection * u_view * position;
    vPosition = position.xyz;
    vNormal = normal;
    vTexCoord = textureCoord;
}