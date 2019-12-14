#include "Particle.h"

using CodeMonkeys::GranularSimulator::Particle;

Particle::Particle(float particle_radius, glm::vec3 offset_from_body) :
	radius(particle_radius),
	offset_from_body(offset_from_body)
{

}
