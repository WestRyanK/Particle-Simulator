#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../Dependencies/stb/stb_image.h"
#include <string.h>

using CodeMonkeys::Engine::Assets::Texture;
using namespace std;


Texture::Texture(const char* texture_path) :
	width(0),
	height(0),
	channel_count(0),
	texture_id(0),
	texture_path(nullptr)
{
    if (texture_path != NULL)
    {
        this->texture_path = texture_path;

        glGenTextures(1, &(this->texture_id)); 
        glBindTexture(GL_TEXTURE_2D, this->texture_id);  

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		int width;
		int height;
		int channel_count;
        unsigned char* data = stbi_load(this->texture_path, &width, &height, &channel_count, 0); 
        if (data)
        {
			this->width = (unsigned int)width;
			this->height = (unsigned int)height;
			this->channel_count = (unsigned int)channel_count;

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->width, this->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glBindTexture(GL_TEXTURE_2D, 0);
            // glGenerateMipmap(GL_TEXTURE_2D);
            stbi_image_free(data);
        }
        else // if (texture_path != NULL)
        {
            printf("Failed to load texture image: %s\n", texture_path);
        }
    }
}

unsigned int Texture::get_width()
{
    return this->width;
}

unsigned int Texture::get_height()
{
    return this->height;
}

unsigned int Texture::get_channel_count()
{
    return this->channel_count;
}

unsigned int Texture::get_texture_id()
{
    return this->texture_id;
}

const char* Texture::get_texture_path()
{
    return this->texture_path;
}

void Texture::update(float dt)
{

}

void Texture::reset()
{

}

Texture* Texture::clone()
{
    Texture* texture_clone = new Texture(NULL);
    texture_clone->width = this->width;
    texture_clone->height = this->height;
    texture_clone->channel_count = this->channel_count;
    texture_clone->texture_id = this->texture_id;
    texture_clone->texture_path = this->texture_path;
    return texture_clone;
}