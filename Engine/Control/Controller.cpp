#include "../Control/Controller.h"

#include "../Control/IControllable.h"

using CodeMonkeys::Engine::Control::Controller;
using namespace CodeMonkeys::Engine::Control;


Controller::Controller(IControllable* controllable, GLFWwindow* window)
{
	this->controllable = controllable;
	this->window = window;
}