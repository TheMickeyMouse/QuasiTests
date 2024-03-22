#version 330 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec3 texcoord;
layout (location = 2) in vec3 normal;

out vec2 vTexCoord;
out vec3 vPosition, vNormal;

uniform mat4 u_projection, u_view;

void main() {
    gl_Position = u_projection * u_view * position;
    vTexCoord = texcoord.xy;
    vPosition = position.xyz;
    vNormal = normal;
}