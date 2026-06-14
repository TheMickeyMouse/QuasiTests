#version 330 core

layout (location = 0) out vec4 glColor;

in vec2 vTexCoord;
in vec3 vPosition, vNormal;

uniform sampler2D boxTex;
uniform vec3 lightPosition;
uniform float ambientStrength;

void main() {
    vec3 lightDir = normalize(lightPosition - vPosition);
    float diffuse = max(dot(lightDir, vNormal), 0);
    glColor = vec4(texture(boxTex, vTexCoord).rgb * (diffuse + ambientStrength), 1.0f);
}