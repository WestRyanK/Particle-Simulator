#ifndef _GRAIN_BODY_OFFSETS_H_
#define _GRAIN_BODY_OFFSETS_H_

#include <vector>
#include <glm/glm.hpp>

namespace CodeMonkeys::GranularSimulator
{
	struct Body;
	struct Particle;
}

namespace CodeMonkeys::GranularSimulator
{
	class BodyGenerator
	{
	private:
		static const float CUBE_GRAIN_CORNER_PARTICLE_RATIO;
		static const float TETRAHEDRON_GRAIN_PARTICLE_RATIO;

	public:
		static glm::vec3 generate_tetrahedron_grain(float base_particle_radius, float particle_density, Body& out_body, std::vector<Particle>& out_particles);
		static glm::vec3 generate_cube_grain(float base_particle_radius, float particle_density, Body& out_body, std::vector<Particle>& out_particles);
		static glm::vec3 generate_sphere_grain(float base_particle_radius, float particle_density, Body& out_body, std::vector<Particle>& out_particles);
		static glm::vec3 generate_plane(float base_particle_radius, float particle_density, glm::vec3 corner_a, glm::vec3 corner_b, glm::vec3 corner_c, Body& out_body, std::vector<Particle>& out_particles);

		static glm::vec3 calculate_center_of_mass(std::vector<Particle> particles);
		static glm::mat3 calculate_inverse_inertial_moment(std::vector<Particle> particles, glm::vec3 center_of_mass, float particle_density);
		static float calculate_total_body_mass(std::vector<Particle> particles, float particle_density);
		static float calculate_particle_mass(float particle_radius, float particle_density);
	};
}

#endif
