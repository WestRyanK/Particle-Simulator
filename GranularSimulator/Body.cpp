#include "Body.h"

using CodeMonkeys::GranularSimulator::Body;

Body::Body()
{
}

Body::Body(float total_mass, glm::mat3 inverse_inertial_moment, bool is_movable, bool is_visible) :
	total_mass(total_mass),
	inverse_inertial_moment(inverse_inertial_moment),
	is_movable(is_movable),
	is_visible(is_visible)
{

}