#pragma once 

#include "../Assets/Texture.h"

namespace CodeMonkeys::Engine::Assets
{
    class AnimatedTexture : public Texture
    {
    private:
        Texture** frames = nullptr;
        unsigned int frame_count = 0;
        unsigned int current_frame = 0;
        float current_time = 0; 
        float animation_speed = 30;
    public:
        AnimatedTexture(const char* filename, const char* extension, unsigned int count);
        void set_current_frame(int frame);
        unsigned int get_current_frame();
        unsigned int get_frame_count();

        unsigned int get_width();
        unsigned int get_height();
        unsigned int get_channel_count();
        unsigned int get_texture_id();
        const char* get_texture_path();
        void set_animation_speed(float animation_speed);
        virtual void update(float dt);
        virtual void reset();
        virtual Texture* clone();
    };
}