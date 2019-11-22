#pragma once
#include "../../Engine/Control/Controller.h"
#include "../../Engine/Control/IControllable.h"

using namespace CodeMonkeys::Engine::Control;

namespace CodeMonkeys::GranularSimulator::Control
{
    class KeyboardController : public Controller
    {
    private:
    public:
        void handle_input(float dt);
        KeyboardController(IControllable* controllable, GLFWwindow* window);
    };
}