#ifndef _BODY_H_
#define _BODY_H_

#include <set>
#include <vector>
#include <glm/glm.hpp>

namespace CodeMonkeys::GranularSimulator
{
	struct Body
	{
		Body();
		Body(float total_mass, glm::mat3 inverse_inertial_moment, bool is_movable = true, bool is_visible = true);
		std::set<int> particle_indices;
		bool is_movable;
		bool is_visible;
		float total_mass;
		glm::mat3 inverse_inertial_moment;
	};
}

#endif
