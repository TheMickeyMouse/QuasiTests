#version 330 core

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in vec3 gNormal[];
out vec4 vColor;

uniform vec4 normalColor;
uniform float normalMagnitude;

uniform mat4 u_projection;

void add_vertex(vec4 pos) {
    gl_Position = u_projection * pos;
    EmitVertex();
}

void add_line(vec4 from, vec4 to) {
    add_vertex(from);
    add_vertex(to);
    EndPrimitive();
}

void main() {
    vColor = normalColor;
    add_line(gl_in[0].gl_Position, gl_in[0].gl_Position + vec4(gNormal[0] * normalMagnitude, 0.0));
    add_line(gl_in[1].gl_Position, gl_in[1].gl_Position + vec4(gNormal[1] * normalMagnitude, 0.0));
    add_line(gl_in[2].gl_Position, gl_in[2].gl_Position + vec4(gNormal[2] * normalMagnitude, 0.0));
}