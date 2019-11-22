#pragma once

#include "../Engine/Dependencies/OpenGL.h"

namespace CodeMonkeys::GranularSimulator
{
    class GranularSimulatorWindow
    {
    private:
        GLFWwindow* window;
        float width;
        float height;

    public:
        GranularSimulatorWindow(int width, int height, bool fullscreen);
        GLFWwindow* get_window();
        float get_width();
        float get_height();
    };
}