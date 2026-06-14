#version 330 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec3 normal;

out VS_OUT {
    vec3 Position;
    vec3 Normal;
    vec4 Color;
    vec4 LightSpace;
} vOut;

uniform mat4 u_projection, u_view, lightSpaceMat;

void main() {
    gl_Position = u_projection * u_view * position;
    vOut.Position = position.xyz;
    vOut.Normal = normal;
    vOut.Color = color;
    vOut.LightSpace = lightSpaceMat * position;
}
