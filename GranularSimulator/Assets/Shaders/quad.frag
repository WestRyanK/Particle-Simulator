#version 330 core

in vec2 uv_2;

out vec4 color_out;

uniform sampler2D quad_texture;

void main(){
    color_out = texture(quad_texture, uv_2);
}