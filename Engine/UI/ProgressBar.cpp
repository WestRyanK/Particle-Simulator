#include "../UI/ProgressBar.h"

#include "../Assets/AnimatedTexture.h"
#include <algorithm>

using CodeMonkeys::Engine::UI::ProgressBar;


ProgressBar::ProgressBar(AnimatedTexture* texture, vec2 position, vec2 size) : Quad(position, size)
{
    this->texture = texture;
    this->set_texture(this->texture->get_texture_id());
}

void ProgressBar::update(float dt)
{
    Quad::update(dt);
}

void ProgressBar::update(float current_value, float total, bool reverse)
{
    current_value = std::max(0.0f, current_value);
    float percent_progress = current_value / (float)total;
    unsigned int frame = (unsigned int)ceil(percent_progress * (this->texture->get_frame_count() - 1));
    if (reverse)
    {
        frame = (this->texture->get_frame_count() - 1) - frame;
    }

    if (frame > this->texture->get_frame_count())
        frame = this->texture->get_frame_count();
    if (frame < 0)
        frame = 0;

    this->texture->set_current_frame(frame);
    this->set_texture(this->texture->get_texture_id());
}