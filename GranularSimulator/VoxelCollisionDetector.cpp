#include "VoxelCollisionDetector.h"

VoxelCollisionDetector::VoxelCollisionDetector(float grid_size) :
    grid_size(grid_size)
{

}

glm::ivec3 VoxelCollisionDetector::get_voxel_key(glm::vec3 position)
{
	glm::ivec3 key = glm::floor(position / this->grid_size);
	return key;
}

void VoxelCollisionDetector::clear()
{
    this->particle_indices_in_voxel.clear();
}

void VoxelCollisionDetector::add_to_voxel_map(unsigned int index, glm::vec3 voxel_key)
{
    this->particle_indices_in_voxel.insert(std::make_pair(voxel_key, index));

}

void VoxelCollisionDetector::remove_from_voxel_map(unsigned int index, glm::vec3 voxel_key)
{
	std::pair<voxelmapiterator, voxelmapiterator> range = this->particle_indices_in_voxel.equal_range(voxel_key);
	voxelmapiterator it = range.first;
	while (it != range.second)
	{
		voxelmapiterator it2 = it;
		it++;
		if (it2->second == index)
		{
			this->particle_indices_in_voxel.erase(it2);
		}
	}
}

void VoxelCollisionDetector::insert(unsigned int index, glm::vec3 position)
{
    glm::ivec3 voxel_key = this->get_voxel_key(position);
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            for (int k = -1; k <= 1; k++)
            {
                glm::ivec3 offset_voxel_key(voxel_key.x + i, voxel_key.y + j, voxel_key.z + k);
                this->add_to_voxel_map(index, offset_voxel_key);
            }
        }
    }
}

void VoxelCollisionDetector::remove(unsigned int index, glm::vec3 position)
{
    glm::ivec3 voxel_key = this->get_voxel_key(position);
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            for (int k = -1; k <= 1; k++)
            {
                glm::ivec3 offset_voxel_key(voxel_key.x + i, voxel_key.y + j, voxel_key.z + k);
                this->remove_from_voxel_map(index, offset_voxel_key);
            }
        }
    }
}

void VoxelCollisionDetector::update(unsigned int index, glm::vec3 old_position, glm::vec3 new_position)
{
	if (this->get_voxel_key(old_position) != this->get_voxel_key(new_position))
	{
		this->remove(index, old_position);
		this->insert(index, new_position);
	}
}

std::unordered_set<unsigned int> VoxelCollisionDetector::get_indices_in_voxel(glm::vec3 position)
{
	glm::ivec3 voxel_key = this->get_voxel_key(position);
	std::unordered_set<unsigned int> indices;

	std::pair<voxelmapiterator, voxelmapiterator> range = this->particle_indices_in_voxel.equal_range(voxel_key);
	voxelmapiterator it = range.first;
	while (it != range.second)
	{
		indices.insert(it->second);
		it++;
	}
	
	return indices;
}
