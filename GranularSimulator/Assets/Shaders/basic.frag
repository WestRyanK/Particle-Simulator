#version 330 core
#define MAX_DIRECTIONAL 4
#define MAX_AMBIENT 1

in vec3 world_position_2;
in vec3 normal_2;
in vec2 uv_2;
flat in int instanceID;

layout(location = 0) out vec4 color_out;

uniform mat4 view_transform;
uniform vec3 camera_position;
uniform sampler2D texture_0;
uniform int material_type_id;
uniform int use_phong_highlight;
uniform float phong_exponent;
uniform vec3 phong_color;
uniform vec3 object_color;
uniform bool is_instanced;


vec4 instanced_colors[12] = vec4[] (
vec4(0.8, 0.71, 0.48, 1.0),
vec4(0.79, 0.78, 0.54, 1.0),
vec4(0.85, 0.74, 0.58, 1.0),
vec4(0.83, 0.75, 0.53, 1.0),
vec4(0.87, 0.72, 0.50, 1.0),
vec4(0.82, 0.79, 0.58, 1.0),
vec4(0.84, 0.67, 0.57, 1.0),
vec4(0.92, 0.77, 0.54, 1.0),
vec4(0.88, 0.76, 0.51, 1.0),
vec4(0.86, 0.81, 0.62, 1.0),
vec4(0.84, 0.73, 0.58, 1.0),
vec4(0.83, 0.75, 0.51, 1.0)

//	vec4(1.0, 0.4, 0.4, 1.0),
//	vec4(1.0, 0.7, 0.4, 1.0),
//	vec4(1.0, 1.0, 0.4, 1.0),
//	vec4(0.7, 1.0, 0.4, 1.0),
//	vec4(0.4, 1.0, 0.4, 1.0),
//	vec4(0.4, 1.0, 0.4, 1.0),
//	vec4(0.4, 1.0, 0.7, 1.0),
//	vec4(0.4, 1.0, 1.0, 1.0),
//	vec4(0.4, 0.7, 1.0, 1.0),
//	vec4(0.4, 0.4, 1.0, 1.0),
//	vec4(0.7, 0.4, 1.0, 1.0),
//	vec4(1.0, 0.4, 1.0, 1.0)
);

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
	if (!is_instanced)
	{
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
	}
	else
	{
		fragment_color = instanced_colors[instanceID % 12];
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