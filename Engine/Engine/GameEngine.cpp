#include "../Engine/GameEngine.h"

#include "../Assets/ShaderProgram.h"
#include "../Collision/ICollisionDetector.h"
#include "../Collision/ICollisionResponse.h"
#include "../Control/Controller.h"
#include "../Control/IControllable.h"
#include "../Engine/Quad.h"
#include "../Engine/Renderer.h"
#include "../Engine/UpdateObjectsIterator.h"
#include "../Events/EventDispatcher.h"
#include "../Objects/Camera3D.h"
#include "../Objects/ILight3D.h"
#include "../Objects/Object3D.h"
#include <stdexcept>
#include <typeinfo>

using CodeMonkeys::Engine::Engine::GameEngine;
using namespace CodeMonkeys::Engine::Assets;
using namespace CodeMonkeys::Engine::Control;
using namespace CodeMonkeys::Engine::Engine;
using namespace CodeMonkeys::Engine::Events;
using namespace CodeMonkeys::Engine::Objects;


GameEngine::GameEngine(GLFWwindow* window, GLuint width, GLuint height) :
	window(window),
	width(width),
	height(height),
	skybox(nullptr),
	camera(nullptr),
	renderer(nullptr),
	world_root(new Object3D(nullptr, "world_root")),
	update_objects_iterator(new UpdateObjectsIterator())
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // GL_LINE or GL_FILL
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}


GLuint GameEngine::get_width()
{
	return this->width;
}

GLuint GameEngine::get_height()
{
	return this->height;
}

void GameEngine::set_width(GLuint width)
{
	this->width = width;
}

void GameEngine::set_height(GLuint height)
{
	this->height = height;
}

GLFWwindow* GameEngine::get_window()
{
	return this->window;
}

void GameEngine::handle_controllers(float dt)
{
	for (Controller* controller : this->controllers)
	{
		controller->handle_input(dt);
	}
}

void GameEngine::draw()
{
	if (this->renderer != NULL)
		this->renderer->render(this->shaders, this->lights, this->camera, this->world_root, this->skybox, this->quads);
	else
		throw "Must specify renderer for engine!";
}

void GameEngine::update_objects(float dt)
{
	this->update_objects_iterator->update(this->world_root, dt);

	for (auto iter = this->quads.begin(); iter != this->quads.end(); iter++)
	{
		(*iter)->update(dt);
	}
}

void GameEngine::handle_collisions(float dt)
{
	if (this->collision_detector != NULL)
	{
		auto collisions = this->collision_detector->get_collisions();

		for (pair<Object3D*, Object3D*> collision : collisions)
		{
			// printf("Collision between %s and %s!\n", collision.first->get_name().c_str(), collision.second->get_name().c_str());

			for (ICollisionResponse* collision_response : this->collision_responses)
			{
				if (collision_response->can_respond(collision.first, collision.second))
				{
					collision_response->respond(collision.first, collision.second, dt);
				}
			}
		}
	}
}

void GameEngine::run()
{
	if (this->collision_detector == NULL)
	{
		printf("Collision Detector NULL!\n");
	}

	this->is_running = true;
	float dt = this->stopwatch.get_seconds_from_last_measurement();
	while (this->is_running)
	{
		dt = this->stopwatch.get_seconds_from_last_measurement();
		if (glfwGetKey(this->window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			this->is_running = false;

		this->update_frame(dt);
		EventDispatcher::get_instance().dispatch();
		glfwPollEvents();
	}
}

void GameEngine::set_collision_detector(ICollisionDetector* collision_detector)
{
	this->collision_detector = collision_detector;
	this->update_objects_iterator->set_collision_detector(collision_detector);
}

Object3D* GameEngine::get_world_root()
{
	return this->world_root;
}