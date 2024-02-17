#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in float textureID;

out vec2 v_TexCoord;
flat out int v_index;

uniform mat4 u_MVP;

void main() {
    gl_Position = u_MVP * position;
    v_TexCoord = texCoord;
    v_index = int(textureID);
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;
flat in int v_index;

uniform sampler2D u_Texture[2];

void main() {
    vec4 texColor = texture(u_Texture[v_index], v_TexCoord);
    color = texColor;
}