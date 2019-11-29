#version 330 core
layout (location = 0) in vec3 position_1;
layout (location = 1) in vec3 normal_1;
layout (location = 2) in vec2 uv_1;
layout (location = 3) in mat4 instanced_transform;

uniform mat4 view_transform;
uniform mat4 projection_transform;
uniform mat4 object_transform;
uniform bool is_instanced;

out vec3 world_position_2;
out vec3 normal_2;
out vec2 uv_2;
flat out int instanceID;

void main()
{
	instanceID = gl_InstanceID;

    normal_2 = (object_transform * vec4(normal_1, 0.0f)).xyz ;
    uv_2 = uv_1;
	if (!is_instanced)
	{
		world_position_2 = (object_transform * vec4(position_1, 1.0f)).xyz;
		gl_Position = projection_transform * view_transform * object_transform * vec4(position_1, 1.0);
	}
	else
	{
		world_position_2 = (instanced_transform * object_transform * vec4(position_1, 1.0f)).xyz;
		gl_Position = projection_transform * view_transform * instanced_transform * object_transform * vec4(position_1, 1.0);
	}
}
