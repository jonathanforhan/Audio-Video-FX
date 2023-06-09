#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 tex_coord;

out vec2 v_tex_coord;

void main() {
    gl_Position = vec4(pos, 1.0);
    v_tex_coord = vec2(tex_coord.x, -tex_coord.y); // flip on gpu
};
