#version 330 core

in vec3 world_position_2;
in vec3 normal_2;
in vec2 uv_2;

layout(location = 0) out vec4 color_out;

uniform sampler2D texture_0;
uniform int material_type_id;
uniform vec3 object_color;

void main()
{
    vec4 fragment_color;
    if (material_type_id == 1)
    {
        fragment_color = vec4(object_color, 1.0f);
    }
    else if (material_type_id == 2)
    {
        fragment_color = texture(texture_0, uv_2);
    }
    else
    {
        fragment_color = vec4(0.5f, 0.5f, 0.5f, 1.0f);
    }

    color_out = fragment_color;
}