#include "GranularSimulatorWindow.h"

#include "../Engine/Dependencies/OpenGL.h"

using CodeMonkeys::GranularSimulator::GranularSimulatorWindow;

GranularSimulatorWindow::GranularSimulatorWindow(int width, int height, bool fullscreen)
{
    // Load GLFW and Create a Window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    GLFWwindow* window = NULL;
    if (fullscreen)
    {
        const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

        window = glfwCreateWindow(mode->width, mode->height, "Granular Simulator", glfwGetPrimaryMonitor(), nullptr);
        this->width = (float)mode->width;
        this->height = (float)mode->height;
    }
    else
    {
        window = glfwCreateWindow(width, height, "Granular Simulator", nullptr, nullptr);
        this->width = (float)width;
        this->height = (float)height;
    }

    // Check for Valid Context
    if (window == nullptr) {
        fprintf(stderr, "Failed to Create OpenGL Context");
        exit(EXIT_FAILURE);
    }

    // Create Context and Load OpenGL Functions
    glfwMakeContextCurrent(window);
    gladLoadGL();
    fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));

    glfwSetWindowPos(window, 0, 0);
    this->window = window;
}

GLFWwindow* GranularSimulatorWindow::get_window()
{
    return this->window;
}

float GranularSimulatorWindow::get_width()
{
    return this->width;
}

float GranularSimulatorWindow::get_height()
{
    return this->height;
}