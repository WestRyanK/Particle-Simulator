#pragma once
#include "../../Engine/Control/Controller.h"
#include "../../Engine/Control/IControllable.h"

using namespace CodeMonkeys::Engine::Control;

namespace CodeMonkeys::GranularSimulator::Control
{
    class MouseController : public Controller
    {
    private:
        float position_x_previous;
        float position_y_previous;
    public:
        MouseController(IControllable* controllable, GLFWwindow* window);
        void get_mouse_delta(float* delta_x, float* delta_y);
        void handle_input(float dt);
    };
}