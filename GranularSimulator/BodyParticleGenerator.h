#ifndef _GRAIN_BODY_OFFSETS_H_
#define _GRAIN_BODY_OFFSETS_H_

#include <vector>
#include <glm/glm.hpp>

namespace CodeMonkeys::GranularSimulator
{
	class BodyParticleGenerator
	{
	private:
		static const float CUBE_GRAIN_CORNER_PARTICLE_RATIO;
		static const float TETRAHEDRON_GRAIN_PARTICLE_RATIO;
	public:
		static std::vector<glm::vec3> get_tetrahedron_grain_offsets(float base_particle_size);
		static std::vector<glm::vec3> get_cube_grain_offsets(float base_particle_size);
		static std::vector<glm::vec3> get_sphere_grain_offsets(float base_particle_size);

		static std::vector<float> get_tetrahedron_grain_sizes(float base_particle_size);
		static std::vector<float> get_cube_grain_sizes(float base_particle_size);
		static std::vector<float> get_sphere_grain_sizes(float base_particle_size);
	};
}

#endif
