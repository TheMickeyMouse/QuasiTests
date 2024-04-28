#version 330 core

layout (location = 0) in vec2 position;

#define CIRCLE_COUNT 100

uniform mat4 u_projection;
uniform float scales[CIRCLE_COUNT];
uniform vec2 offsets[CIRCLE_COUNT];
uniform vec4 colors[CIRCLE_COUNT];
uniform int selected;

out vec4 vColor;

void main() {
    gl_Position = u_projection * (vec4(position * scales[gl_InstanceID] + offsets[gl_InstanceID], 0.0, 1.0));
    vColor = (gl_InstanceID == selected) ? vec4(1.0) : colors[gl_InstanceID];
}
