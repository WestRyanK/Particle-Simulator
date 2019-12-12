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
		float simulation_duration;
		float current_time;
		unsigned int particle_count;
		float play_speed;

		void spawn_particles(ShaderProgram* shader);


	public:
		GranularSubstance(Model3D* model, ShaderProgram* shader);
		virtual void update(float dt);

		// Inherited via IControllable
		virtual void control(std::string control_name, float value, float dt) override;
		virtual void control(std::string control_name, int value, float dt) override;
	};
}

#endif

