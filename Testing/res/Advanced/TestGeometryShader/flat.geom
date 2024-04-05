#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec4 vColor[];
out vec3 gNormal;
out vec4 gColor;

uniform int faceIndex;
uniform mat4 u_projection, u_view, u_model;

void add_vertex(vec4 pos) {
    gl_Position = u_projection * u_view * u_model * pos;
    EmitVertex();
}

void main() {
    if (faceIndex == -1 || (gl_PrimitiveIDIn / 16) == faceIndex) {
        vec4 a = gl_in[0].gl_Position;
        vec4 b = gl_in[1].gl_Position;
        vec4 c = gl_in[2].gl_Position;

        gNormal = normalize(cross(vec3(b - a), vec3(c - a)));
        gColor = vColor[0];
        add_vertex(a);
        gColor = vColor[1];
        add_vertex(b);
        gColor = vColor[2];
        add_vertex(c);
        EndPrimitive();
    }
}
