#ifndef _GRANULAR_SIMULATOR_PARTICLE_H_
#define _GRANULAR_SIMULATOR_PARTICLE_H_

#include <glm/glm.hpp>

namespace CodeMonkeys::GranularSimulator
{
	struct Particle
	{
		Particle(float particle_radius, glm::vec3 offset_from_body);

		unsigned int body_index;
		float radius;
		glm::vec3 offset_from_body;
	};
}

#endif
