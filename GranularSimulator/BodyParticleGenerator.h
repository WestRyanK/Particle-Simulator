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
		static void get_tetrahedron_grain(float base_particle_size, float particle_mass_to_size_ratio, std::vector<glm::vec3>& out_particle_offsets, std::vector<float>& out_particle_sizes, glm::vec3& center_of_mass, float& body_mass, glm::mat3& inertial_moment);
		static void get_cube_grain(float base_particle_size, float particle_mass_to_size_ratio, std::vector<glm::vec3>& out_particle_offsets, std::vector<float>& out_particle_sizes, glm::vec3& center_of_mass, float& body_mass, glm::mat3& inertial_moment);
		static void get_sphere_grain(float base_particle_size, float particle_mass_to_size_ratio, std::vector<glm::vec3>& out_particle_offsets, std::vector<float>& out_particle_sizes, glm::vec3& center_of_mass, float& body_mass, glm::mat3& inertial_moment);
		static void get_plane(float base_particle_size, float particle_mass_to_size_ratio, glm::vec3 corner_a, glm::vec3 corner_b, glm::vec3 corner_c, std::vector<glm::vec3>& out_particle_offsets, std::vector<float>& out_particle_sizes, glm::vec3& center_of_mass, float& body_mass, glm::mat3& inertial_moment);

		static glm::vec3 calculate_center_of_mass(std::vector<glm::vec3> particle_offsets);
		static glm::mat3 calculate_inertial_moment(std::vector<glm::vec3> particle_offsets, glm::vec3 center_of_mass, float particle_mass_to_size_ratio, std::vector<float> particle_sizes);
		static float calculate_total_body_mass(std::vector<float> particle_sizes, float particle_mass_to_size_ratio);
	};
}

#endif
