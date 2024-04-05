#version 330 core

layout (location = 0) out vec4 glColor;

in vec3 vNormal;
in vec4 vColor;

uniform vec3 lightDirection;
uniform float ambientStrength;

void main() {
    float diffuse = max(dot(vNormal, lightDirection), 0.0);
    glColor = vec4(vColor.rgb * (ambientStrength + diffuse), vColor.a);
}