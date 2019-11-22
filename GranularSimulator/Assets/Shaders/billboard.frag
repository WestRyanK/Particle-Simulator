#version 330 core

in vec2 uv_2;

out vec4 color_out;

uniform sampler2D billboard_texture;

void main(){
    color_out = texture(billboard_texture, uv_2);
    // if (color_out.a < 0.5)
    //     discard;
}