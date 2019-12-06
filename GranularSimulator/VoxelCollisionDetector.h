#pragma once

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"
#include <unordered_map>
#include <unordered_set>
#include <string>

typedef std::unordered_multimap<glm::ivec3, unsigned int>	voxelmap;
typedef std::unordered_multimap<glm::ivec3, unsigned int>::iterator voxelmapiterator;

typedef std::unordered_map<unsigned int, glm::ivec3> reversevoxelmap;
typedef std::unordered_map<unsigned int, glm::ivec3>::iterator reversevoxelmapiterator;

class VoxelCollisionDetector 
{
private:
	float grid_size;
	voxelmap particle_indices_in_voxel;
	reversevoxelmap voxel_for_particle_index;
	void add_to_voxel_map(unsigned int index, glm::ivec3 voxel_key);
	void remove_from_voxel_map(unsigned int index, glm::ivec3 voxel_key);
public:
	VoxelCollisionDetector(float grid_size);

	void clear();
	void insert(unsigned int index, glm::vec3 position);
	void remove(unsigned int index);
	void update(unsigned int index, glm::vec3 new_position);
	
	std::unordered_set<unsigned int> get_indices_in_voxel(glm::vec3 position);

	glm::ivec3 get_voxel_key(glm::vec3 position) const;
};