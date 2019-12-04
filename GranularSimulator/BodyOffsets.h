#ifndef _GRAIN_BODY_OFFSETS_H_
#define _GRAIN_BODY_OFFSETS_H_

#include <vector>
#include <glm/glm.hpp>

namespace CodeMonkeys::GranularSimulator
{
	class BodyOffsets
	{
	private:
		static std::vector<glm::vec3>* tetrahedron_grain;
		static std::vector<glm::vec3>* cube_grain;
		static std::vector<glm::vec3>* sphere_grain;
	public:
		static std::vector<glm::vec3>* get_tetrahedron_grain(float particle_size);
		static std::vector<glm::vec3>* get_cube_grain(float particle_size);
		static std::vector<glm::vec3>* get_sphere_grain(float particle_size);
	};
}

#endif
