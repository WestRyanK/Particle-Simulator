#include "KeyboardController.h"

using CodeMonkeys::GranularSimulator::Control::KeyboardController;

KeyboardController::KeyboardController(IControllable* controllable, GLFWwindow* window) : Controller(controllable, window) 
{ 
}


void KeyboardController::handle_input(float dt)
{
    glfwPollEvents();
    if (glfwGetKey(this->window, GLFW_KEY_S) == GLFW_PRESS)
    {
        this->controllable->control("move_z", -1.0f, dt);
    }
    if (glfwGetKey(this->window, GLFW_KEY_W) == GLFW_PRESS)
    {
        this->controllable->control("move_z", 1.0f, dt);
    }
    if (glfwGetKey(this->window, GLFW_KEY_W) == GLFW_RELEASE && glfwGetKey(this->window, GLFW_KEY_S) == GLFW_RELEASE)
    {
        this->controllable->control("move_z", 0.0f, dt);
    }
    if (glfwGetKey(this->window, GLFW_KEY_A) == GLFW_PRESS)
    {
        this->controllable->control("move_x", 1.0f, dt);
    }
    if (glfwGetKey(this->window, GLFW_KEY_D) == GLFW_PRESS)
    {
        this->controllable->control("move_x", -1.0f, dt);
    }
    if (glfwGetKey(this->window, GLFW_KEY_D) == GLFW_RELEASE && glfwGetKey(this->window, GLFW_KEY_A) == GLFW_RELEASE)
    {
        this->controllable->control("move_x", 0.0f, dt);
    }

    if (glfwGetKey(this->window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        this->controllable->control("move_y", -1.0f, dt);
    }
    if (glfwGetKey(this->window, GLFW_KEY_E) == GLFW_PRESS)
    {
        this->controllable->control("move_y", 1.0f, dt);
    }
    if (glfwGetKey(this->window, GLFW_KEY_Q) == GLFW_RELEASE && glfwGetKey(this->window, GLFW_KEY_E) == GLFW_RELEASE)
    {
        this->controllable->control("move_y", 0.0f, dt);
    }

    if (glfwGetKey(this->window, GLFW_KEY_J) == GLFW_PRESS)
    {
        this->controllable->control("play_speed", -1.0f, dt);
    }
    if (glfwGetKey(this->window, GLFW_KEY_K) == GLFW_PRESS)
    {
        this->controllable->control("play_speed", 1.0f, dt);
    }

	if (glfwGetKey(this->window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		this->controllable->control("reset", 1, dt);
	}
	else
	{
		this->controllable->control("reset", 0, dt);
	}
}
