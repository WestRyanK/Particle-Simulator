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
		static void get_tetrahedron_grain(float base_particle_size, std::vector<glm::vec3>& out_particle_offsets, std::vector<float>& out_particle_sizes);
		static void get_cube_grain(float base_particle_size, std::vector<glm::vec3>& out_particle_offsets, std::vector<float>& out_particle_sizes);
		static void get_sphere_grain(float base_particle_size, std::vector<glm::vec3>& out_particle_offsets, std::vector<float>& out_particle_sizes);

		static void get_plane(float base_particle_size, glm::vec3 corner_a, glm::vec3 corner_b, glm::vec3 corner_c, std::vector<glm::vec3>& out_particle_offsets, std::vector<float>& out_particle_sizes);
	};
}

#endif
