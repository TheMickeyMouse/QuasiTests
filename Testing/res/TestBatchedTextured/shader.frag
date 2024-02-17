#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;
in vec4 v_color;
flat in int v_index;

uniform sampler2D u_textures[2];

void main(){
    vec4 texColor = texture(u_textures[v_index], v_TexCoord);
    color = v_color * texColor;
}