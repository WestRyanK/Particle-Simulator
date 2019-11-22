#version 330 core
#define MAX_DIRECTIONAL 4
#define MAX_AMBIENT 1

in vec3 world_position_2;
in vec3 normal_2;
in vec2 uv_2;

layout(location = 0) out vec4 color_out;

uniform mat4 view_transform;
uniform vec3 camera_position;
uniform sampler2D texture_0;
uniform int material_type_id;
uniform int use_phong_highlight;
uniform float phong_exponent;
uniform vec3 phong_color;
uniform vec3 object_color;

uniform int directional_count;
uniform struct directional_light
{
    float strength;
    vec3 color;
    vec3 direction;
} directional[MAX_DIRECTIONAL];

uniform int ambient_count;
uniform struct ambient_light
{
    float strength;
    vec3 color;
} ambient[MAX_AMBIENT];

vec4 apply_ambient(vec4 fragment_color, ambient_light light)
{
    vec4 ambient_out = fragment_color * light.strength * vec4(light.color, 1.0f);
    return ambient_out;
}

vec4 apply_directional(vec4 fragment_color, directional_light light)
{

    vec4 directional_out = fragment_color * vec4(light.color * light.strength * max(0.0f, dot(normal_2, -light.direction)), 1.0f);

    if (use_phong_highlight == 1)
    {
        vec3 r = normalize(2 * normal_2 * dot(normal_2, -light.direction) + light.direction);

        vec3 e = normalize(camera_position - world_position_2 );
        float phong_base = max(0.0f, dot(e, r));
        if (phong_base > 0.0f)
        {
            vec4 phong_reflect_out = vec4(light.color * phong_color * pow(phong_base, phong_exponent), 1.0f);
            directional_out += phong_reflect_out;
        }

    }
        // directional_out = vec4(normal_2, 1.0f);
    return directional_out;
}

void main()
{
    vec4 fragment_color;
    if (material_type_id == 1)
    {
        fragment_color = vec4(object_color, 1.0f);
    }
    else if (material_type_id == 2)
    {
        fragment_color = vec4(object_color, 1.0f);
        fragment_color = texture(texture_0, uv_2) * fragment_color;
    }
    else
    {
        fragment_color = vec4(1.0f, 0.0f, 0.5f, 1.0f);
    }

    // color_out = vec3(0);
    color_out = vec4(0.0f);
    for (int i = 0; i < ambient_count; i++)
    {
        color_out += apply_ambient(fragment_color, ambient[i]);
    }
    for (int i = 0; i < directional_count; i++)
    {
        color_out += apply_directional(fragment_color, directional[i]);
    }
}