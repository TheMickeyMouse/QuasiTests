#version 330 core

layout (location = 0) out vec4 glColor;

in vec3 vPosition, vNormal;

uniform samplerCube skybox;
uniform vec3 viewPosition;
uniform float ambStrength;

void main() {
    vec3 viewDir = normalize(vPosition - viewPosition);
    vec3 reflected = reflect(viewDir, vNormal);
    glColor = vec4(texture(skybox, reflected).rgb * (1.0 + ambStrength), 1.0f);
}