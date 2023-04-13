#version 330 core

out vec4 f_color;

in vec2 v_tex_coord;

uniform sampler2D u_texture;

void main() {
    f_color = vec4(1.0f, 0.5f, 0.2f, 1.0f);
    //f_color = texture(u_texture, v_tex_coord);
};

