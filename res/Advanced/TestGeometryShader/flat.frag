#version 330 core

layout (location = 0) out vec4 glColor;

in vec3 gPosition;
in vec3 gNormal;
in vec4 gColor;

uniform float ambientStrength;

void main() {
    float diffuse = max(-dot(gNormal, normalize(gPosition)), 0.0);
    glColor = vec4(gColor.rgb * (ambientStrength + diffuse), gColor.a);
}
