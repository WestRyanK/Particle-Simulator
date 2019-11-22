#ifndef _PROGRESS_BAR_H_
#define _PROGRESS_BAR_H_

#include "../Dependencies/OpenGL.h"
#include "../Engine/Quad.h"

namespace CodeMonkeys::Engine
{
	namespace Assets
	{
		class AnimatedTexture;
	}
}

using namespace CodeMonkeys::Engine::Assets;
using namespace CodeMonkeys::Engine::Engine;
using namespace glm;


namespace CodeMonkeys::Engine::UI
{
    class ProgressBar : public Quad
    {
    protected:
        AnimatedTexture* texture = NULL;
        void update(float current_value, float total, bool reverse);
    public:
        ProgressBar(AnimatedTexture* texture, vec2 position, vec2 size);
        virtual void update(float dt);
    };
}

#endif