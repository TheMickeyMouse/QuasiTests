#version 330

layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in int matId;

out vec3 vPosition;
out vec3 vNormal;
flat out int vMatId;

uniform mat4 u_projection, u_view;

void main() {
    gl_Position = u_projection * u_view * position;
    vPosition = position.xyz;
    vNormal = normal;
    vMatId = matId;
}