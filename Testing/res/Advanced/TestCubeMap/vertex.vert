#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 texcoord;
layout (location = 2) in vec3 normal;

out vec3 vTexCoord;

uniform mat4 u_projection, u_view;

void main() {
    // gl_Position = vec4(vec3(u_projection * u_view * vec4(position, 0.0)), 1.0);
    vec4 pos = u_projection * u_view * vec4(position, 0.0);
    gl_Position = pos.xyzz; // z / z is 1, always has a depth of one
    vTexCoord = texcoord;
}