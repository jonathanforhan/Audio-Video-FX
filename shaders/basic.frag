#version 330 core

out vec4 f_color;

in vec2 v_tex_coord;

uniform sampler2D u_texture;

void main() {
    f_color = texture(u_texture, v_tex_coord);
};

