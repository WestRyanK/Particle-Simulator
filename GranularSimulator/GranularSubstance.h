#ifndef _SUBSTANCE_SIMULATOR_H_
#define _SUBSTANCE_SIMULATOR_H_

#include "../Engine/Control/IControllable.h"
#include "../Engine/Objects/InstancedObject3D.h"
#include "GranularSubstanceSimulator.h"
#include <vector>

namespace CodeMonkeys::Engine
{
	namespace Assets
	{
		class Texture;
		class Material;
		class Model3D;
		struct mlModel;
	}
}

using namespace CodeMonkeys::Engine::Assets;
using namespace CodeMonkeys::Engine::Control;
using namespace CodeMonkeys::Engine::Objects;
using namespace std;

namespace CodeMonkeys::GranularSimulator
{
	class GranularSubstance : public InstancedObject3D, public IControllable
	{
	private:
		GranularSubstanceSimulator* simulator;
		unsigned int frame_count;
		float current_time;
		unsigned int particle_count;
		float particle_size;
		float timestep_size;
		float play_speed;

		unsigned int current_frame;
		bool finished_simulating;

		void spawn_particles(ShaderProgram* shader);


	public:
		GranularSubstance(Model3D* model, unsigned int frame_count, float timestep_size, unsigned int particle_count, float particle_size, float particle_mass, ShaderProgram* shader);
		virtual void update(float dt);

		// Inherited via IControllable
		virtual void control(std::string control_name, float value, float dt) override;
		virtual void control(std::string control_name, int value, float dt) override;
	};
}

#endif

