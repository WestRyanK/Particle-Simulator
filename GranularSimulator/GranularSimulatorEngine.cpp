#include "GranularSimulatorEngine.h"

#include "../Engine/Assets/ColorMaterial.h"
#include "../Engine/Assets/Model3D.h"
#include "../Engine/Assets/ModelLoader.h"
#include "../Engine/Assets/ShaderProgram.h"
#include "../Engine/Collision/SimpleCollisionDetector.h"
#include "../Engine/Engine/Renderer.h"
#include "../Engine/Objects/AmbientLight.h"
#include "../Engine/Objects/Billboard.h"
#include "../Engine/Objects/Camera3D.h"
#include "../Engine/Objects/DirectionalLight.h"
#include "../Engine/Objects/Object3D.h"
#include "../Engine/Objects/Particle.h"
#include "../Engine/Objects/ParticleEmitter.h"
#include "../Engine/UI/Text.h"
#include "Control/KeyboardController.h"
#include "Control/MouseController.h"
#include "GranularSubstance.h"
#include "RandomGenerator.h"
#include <stdlib.h>
#include <vector>

using CodeMonkeys::GranularSimulator::GranularSimulatorEngine;
using namespace CodeMonkeys::Engine::Assets;
using namespace CodeMonkeys::Engine::Engine;
using namespace CodeMonkeys::Engine::Objects;
using namespace CodeMonkeys::Engine::UI;
using namespace CodeMonkeys::GranularSimulator::Control;
using namespace CodeMonkeys::GranularSimulator;
using namespace std;


GranularSimulatorEngine::GranularSimulatorEngine(GLFWwindow* window, GLuint width, GLuint height) : GameEngine(window, width, height)
{
}

void GranularSimulatorEngine::update_frame(float dt)
{
	this->handle_controllers(dt);
	this->update_objects(dt);
	this->handle_collisions(dt);
	this->draw();
}

void GranularSimulatorEngine::init_simulator(ShaderProgram* shader)
{
	std::vector<Material*> materials;
	Material* material = new ColorMaterial(shader, false, 20.0f, glm::vec3(1.0f, 1.0f, 1.0f), RandomGenerator::RandomVecBetween(0.0f, 1.0f));
	materials.push_back(material);
	materials.push_back(material);
	mlModel* ml_model = new mlModel();
    LoadModel("Assets", "block.obj", *ml_model);
	Model3D* model = new Model3D(ml_model, materials);

	GranularSubstance* substance = new GranularSubstance(
		model,
		700,			//	frame_count		(#)
		0.002f,			//	timestep_size	(second) (50 frames/second)
		0.04f,			//	particle_size	(meter) 
0.25f,//0.00025f,		//	particle_mass	(kg)  (~1/10th of a penny)
		shader);		//	material
	this->world_root->add_child(substance);

    auto keyboard_controller = new KeyboardController(substance, this->get_window());
	this->controllers.insert(keyboard_controller);
}

void GranularSimulatorEngine::init()
{
	auto shader = new ShaderProgram("Assets/Shaders/basic.vert", "Assets/Shaders/basic.frag");
	auto self_illuminated_shader = new ShaderProgram("Assets/Shaders/basic.vert", "Assets/Shaders/self_illuminated.frag");

	// Make sure that every shader used in the scene is added to the engine's list of shaders so
	// that lighting can be calculated.
	this->shaders.insert(shader);
	this->shaders.insert(self_illuminated_shader);
	Billboard::init_billboard_class();
	this->shaders.insert(Billboard::get_shader());

	this->camera = new Camera3D();
	this->camera->set_position(glm::vec3(0, 0, 1));
	this->camera->set_look_at(glm::vec3(0, 0, 0));

	auto ambient = new AmbientLight(vec3(1.0f, 1.0f, 1.0f), 0.3f);
	this->lights.insert(ambient);

    auto directional = new DirectionalLight(vec3(1.0f, 1.0f, 1.0f), 1.0f, vec3(2, -3, -1));
    this->lights.insert(directional);
    directional = new DirectionalLight(vec3(1.0f, 1.0f, 1.0f), 0.5f, vec3(2, -3, 1));
    this->lights.insert(directional);

	this->set_collision_detector(new SimpleCollisionDetector());

	this->init_simulator(shader);

	std::vector<Material*> materials;
	Material* material = new ColorMaterial(shader, true, 5.0f, glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.2f, 0.2f, 0.8f));
	materials.push_back(material);
    mlModel* ml_model = new mlModel();
    LoadModel("Assets", "block.obj", *ml_model);
	Model3D* model = new Model3D(ml_model, materials);
	Object3D* object = new Object3D(model, "object");
	object->set_scale(glm::vec3(2.0f, 0.04f, 2.0f));
	object->set_position(glm::vec3(0.0f, -0.02f, 0.0f));
	this->world_root->add_child(object);

    auto keyboard_controller = new KeyboardController(this->camera, this->get_window());
    auto mouse_controller = new MouseController(this->camera, this->get_window());
    this->controllers.insert(keyboard_controller);
    this->controllers.insert(mouse_controller);

	this->renderer = new Renderer(this->get_window(), this->get_width(), this->get_height());
}
