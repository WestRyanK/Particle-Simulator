#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec2 position_1;
layout(location = 1) in vec2 uv_1;

uniform vec2 position;
uniform vec2 size;

// Output data ; will be interpolated for each fragment.
out vec2 uv_2;

void main(){
	gl_Position = vec4(position_1, 0, 1) * vec4(size, 0, 1) + vec4(position, 0, 0);
	// uv_2 = (position_1.xy + vec2(1, 1)) / 2.0;
	uv_2 = uv_1;
}