#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "../Dependencies/OpenGL.h"
#include <string>

using namespace std;

namespace CodeMonkeys::Engine::Assets
{
    class Texture
    {
    protected:
        unsigned int width = 0;
        unsigned int height = 0;
        unsigned int channel_count = 0;
        unsigned int texture_id = 0;
        const char* texture_path = nullptr;

    public:
        Texture(const char* texture_path);
        virtual unsigned int get_width();
        virtual unsigned int get_height();
        virtual unsigned int get_channel_count();
        virtual unsigned int get_texture_id();
        virtual const char* get_texture_path();
        virtual void update(float dt);
        virtual void reset();
        virtual Texture* clone();
    };
}

#endif