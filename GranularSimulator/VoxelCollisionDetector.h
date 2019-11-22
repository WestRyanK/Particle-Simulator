#pragma once

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"
#include <unordered_map>
#include <unordered_set>
#include <string>

typedef std::unordered_multimap<glm::vec3, unsigned int>	voxelmap;
typedef std::unordered_multimap<glm::vec3, unsigned int>::iterator voxelmapiterator;

class VoxelCollisionDetector 
{
private:
	float grid_size;
	voxelmap particle_indices_in_voxel;
	void add_to_voxel_map(unsigned int index, glm::vec3 voxel_key);
	void remove_from_voxel_map(unsigned int index, glm::vec3 voxel_key);
public:
	VoxelCollisionDetector(float grid_size);

	void clear();
	void insert(unsigned int index, glm::vec3 position);
	void remove(unsigned int index, glm::vec3 position);
	void update(unsigned int index, glm::vec3 old_position, glm::vec3 new_position);
	
	std::unordered_set<unsigned int> get_indices_in_voxel(glm::vec3 position);

	glm::ivec3 get_voxel_key(glm::vec3 position);
};