#include "MouseController.h"

using CodeMonkeys::GranularSimulator::Control::MouseController;

MouseController::MouseController(IControllable* controllable, GLFWwindow* window) : Controller(controllable, window)
{
    // TODO: Uncomment me in order to hide mouse!
    // glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void MouseController::get_mouse_delta(float* delta_x, float* delta_y)
{
    double position_x, position_y;
    glfwGetCursorPos(this->window, &position_x, &position_y);

    *delta_x = position_x - (float)this->position_x_previous;
    *delta_y = position_y - (float)this->position_y_previous;
    this->position_x_previous = position_x;
    this->position_y_previous = position_y;
}

void MouseController::handle_input(float dt)
{
    float delta_x, delta_y;
    this->get_mouse_delta(&delta_x, &delta_y);

    if (delta_x != 0)
    {
        this->controllable->control("aim_x", delta_x, dt);
    }
    if (delta_y != 0)
    {
        this->controllable->control("aim_y", delta_y, dt);
    }

    if (glfwGetMouseButton(this->window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        this->controllable->control("fire_primary", 1, dt);
    }
    else if (glfwGetMouseButton(this->window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
    {
        this->controllable->control("fire_primary", 0, dt);
    }
    if (glfwGetMouseButton(this->window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        this->controllable->control("fire_secondary", 1, dt);
    }
    else if (glfwGetMouseButton(this->window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
    {
        this->controllable->control("fire_secondary", 0, dt);
    }
}