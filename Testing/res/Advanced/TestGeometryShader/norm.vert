#version 330 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 color;

out vec3 gNormal;

uniform mat4 u_view, u_model, normMat;

void main() {
    gl_Position = u_view * u_model * position;
    gNormal = vec3(normMat * vec4(normal, 0.0));
}