#version 330 core

layout(location = 0) in vec2 position_1;
layout(location = 1) in vec2 uv_1;

uniform mat4 view_transform;
uniform vec3 camera_position;
uniform mat4 projection_transform;
uniform mat4 object_transform;
uniform vec2 billboard_size;

out vec2 uv_2;

void main(){
    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 object_position = vec3(object_transform[3][0], object_transform[3][1], object_transform[3][2]);

    vec3 N = normalize(vec3(camera_position) - object_position);
    vec3 U = cross(up, N);
    vec3 V = cross(N, U);

    mat4 face_camera = mat4(
        U.x, V.x, N.x, 0.0,
        U.y, V.y, N.y, 0.0,
        U.z, V.z, N.z, 0.0,
        0.0, 0.0, 0.0, 1.0);

    face_camera = transpose(face_camera);

    gl_Position = projection_transform * view_transform * object_transform * face_camera * vec4(position_1 * billboard_size, 0.0, 1.0);
	uv_2 = uv_1;
}