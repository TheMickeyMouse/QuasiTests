#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 5) out;

in vec4 gColor[];
out vec4 vColor;

void add_vertex(vec4 pos) {
    gl_Position = pos;
    EmitVertex();
}

void main() {
    vec4 a = gl_in[0].gl_Position;
    vec4 b = gl_in[1].gl_Position;
    vec4 c = gl_in[2].gl_Position;

    vColor = gColor[0];
    add_vertex(a);
    add_vertex(c);
    add_vertex(b);
    add_vertex(b + c - a);
    add_vertex(b * 1.5 + c * 0.5 - a);
    EndPrimitive();
}