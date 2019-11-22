#ifndef _GAME_ENGINE_H_
#define _GAME_ENGINE_H_

#include "../Dependencies/OpenGL.h"
#include "../Engine/Stopwatch.h"
#include <set>

namespace CodeMonkeys::Engine
{
	namespace Assets
	{
		class ShaderProgram;
	}
	namespace Collision
	{
		class ICollisionResponse;
		class ICollisionDetector;
	}
	namespace Control
	{
		class Controller;
		class IControllable;
	}
	namespace Engine
	{
		class Renderer;
		class UpdateObjectsIterator;
		class Quad;
	}
	namespace Objects
	{
		class ILight3D;
		class Camera3D;
		class Object3D;
		class Skybox;
	}
}

using namespace CodeMonkeys::Engine::Assets;
using namespace CodeMonkeys::Engine::Collision;
using namespace CodeMonkeys::Engine::Control;
using namespace CodeMonkeys::Engine::Engine;
using namespace CodeMonkeys::Engine::Objects;
using namespace glm;
using namespace std;

namespace CodeMonkeys::Engine::Engine
{
	class GameEngine
	{
	private:
		GLFWwindow* window;
		Stopwatch stopwatch;
		ICollisionDetector* collision_detector = nullptr;
		UpdateObjectsIterator* update_objects_iterator = nullptr;
		GLuint width;
		GLuint height;

	protected:
		set<Controller*> controllers;
		Object3D* world_root;
		set<ILight3D*> lights;
		set<ShaderProgram*> shaders;
		set<ICollisionResponse*> collision_responses;
		set<Quad*> quads;
		Camera3D* camera;
		Skybox* skybox;
		Renderer* renderer;

		bool is_running = false;

		GLFWwindow* get_window();
		GLuint get_width();
		GLuint get_height();
		void set_width(GLuint width);
		void set_height(GLuint height);
		virtual void set_collision_detector(ICollisionDetector* collision_detector);

		virtual void handle_controllers(float dt);
		virtual void update_objects(float dt);
		virtual void handle_collisions(float dt);
		virtual void update_frame(float dt) = 0;
		void draw();

	public:
		GameEngine(GLFWwindow* window, GLuint width, GLuint height);
		void run();
		virtual void init() = 0;


		Object3D* get_world_root();
	};

}

#endif
