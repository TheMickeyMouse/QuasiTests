#version 330 core

layout (location = 0) out float glDepth;

void main() {
    glDepth = gl_FragCoord.z;
} // we only need depth so this doesnt matter
