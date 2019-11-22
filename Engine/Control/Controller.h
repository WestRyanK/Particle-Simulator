#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include "../Dependencies/OpenGL.h"

namespace CodeMonkeys::Engine
{
	namespace Control
	{
		class IControllable;
	}
}

using namespace CodeMonkeys::Engine::Control;

namespace CodeMonkeys::Engine::Control
{
    class Controller
    {
    protected:
        IControllable* controllable = NULL;
        GLFWwindow* window = NULL;
    public:
        Controller(IControllable* controllable, GLFWwindow* window);
        virtual void handle_input(float dt) = 0;
    };
}

#endif