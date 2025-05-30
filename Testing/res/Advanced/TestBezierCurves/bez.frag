#version 330 core

in vec4 vPosition;
in vec2 vBezCoord;
in vec4 vColor;

void main() {
    if ((vBezCoord.y < vBezCoord.x * vBezCoord.x) ^^ gl_FrontFacing)
        discard;
    gl_FragColor = vColor;
}
