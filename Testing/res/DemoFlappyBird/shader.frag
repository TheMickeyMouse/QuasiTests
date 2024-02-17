#version 330 core

layout(location = 0) out vec4 color;

in vec4 v_color;
in vec2 v_TexCoord;
flat in int v_rtype;

uniform sampler2D u_font;

void main() {
    color = v_color;
    if (v_rtype == 1) {
        float alpha = texture(u_font, v_TexCoord).r;
        alpha = smoothstep(0.492, 0.508, alpha);
        color.a = alpha;
    }
}