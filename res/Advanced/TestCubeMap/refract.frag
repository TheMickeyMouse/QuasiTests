#version 330 core

layout (location = 0) out vec4 glColor;

in vec3 vPosition, vNormal;

uniform samplerCube skybox;
uniform vec3 viewPosition;
uniform float ambStrength, invRefractIndex;

void main() {
    vec3 viewDir = normalize(vPosition - viewPosition);
    vec3 refracted = refract(viewDir, vNormal, invRefractIndex);
    glColor = vec4(texture(skybox, refracted).rgb * (1.0 + ambStrength), 1.0f);
}