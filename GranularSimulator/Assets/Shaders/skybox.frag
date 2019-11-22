#version 330 core
// out vec4 FragColor;
layout(location = 0) out vec4 color_out;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{
    // FragColor = texture(skybox, TexCoords);
    color_out = texture(skybox, TexCoords);
}
