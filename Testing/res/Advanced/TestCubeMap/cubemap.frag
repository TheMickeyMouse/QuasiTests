#version 330 core

layout (location = 0) out vec4 glColor;

in vec3 vTexCoord;

uniform samplerCube cubemap;

void main() {
    glColor = texture(cubemap, vTexCoord);
}