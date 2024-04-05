#version 330 core

layout (location = 0) out vec4 glColor;

in vec3 gNormal;
in vec4 gColor;

uniform vec3 lightDirection;
uniform float ambientStrength;

void main() {
    float diffuse = max(dot(gNormal, lightDirection), 0.0);
    glColor = vec4(gColor.rgb * (ambientStrength + diffuse), gColor.a);
}
