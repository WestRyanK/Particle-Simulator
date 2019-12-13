#include "pch.h"
#include <glm/glm.hpp>
#include "../GranularSimulator/VoxelCollisionDetector.h"
#include "../GranularSimulator/VoxelCollisionDetector.cpp"

TEST(VoxelCollisionDetectorTest, GetVoxelKeyTest)
{
	VoxelCollisionDetector detector(10.0f);
	EXPECT_EQ(glm::ivec3(0, 0, 0), detector.get_voxel_key(glm::vec3(0, 5, 9)));
	EXPECT_EQ(glm::ivec3(0, 1, 1), detector.get_voxel_key(glm::vec3(1, 10, 14)));
	EXPECT_EQ(glm::ivec3(1, 2, 2), detector.get_voxel_key(glm::vec3(17, 22, 20.5f)));
	EXPECT_EQ(glm::ivec3(-1, -1, -1), detector.get_voxel_key(glm::vec3(-1, -0.5f, -7)));
	EXPECT_EQ(glm::ivec3(-2, -3, 4), detector.get_voxel_key(glm::vec3(-16, -21.5f, 45)));
}

// |----------------------------------|
// |-2,-2 |-1,-2 |0,-2  |1,-2  |2,-2  |
// |3     |3     |      |2     |2     |
// |------|------|------|------|------|
// |-2,-1 |-1,-1 |0,-1  |1,-1  |2,-1  |
// |3     |03    |0     |02    |2     |
// |------|------|------|------|------|
// |-2,0  |-1,0  |0,0   |1,0   |2,0   |
// |      |0     |0     |012   |12    |
// |------|------|------|------|------|
// |-2,1  |-1,1  |0,1   |1,1   |2,1   |
// |      |0     |0     |01    |1     |
// |------|------|------|------|------|
// |-2,2  |-1,2  |0,2   |1,2   |2,2   |
// |      |      |      |1     |1     |
// |------|------|------|------|------|

TEST(VoxelCollisionDetectorTest, InsertTest)
{
	VoxelCollisionDetector detector(10.0f);
	detector.insert(0, glm::vec3(0,0,0));
	detector.insert(1, glm::vec3(20,10,0));
	detector.insert(2, glm::vec3(20,-10,0));
	detector.insert(3, glm::vec3(-20,-20,0));


	
	std::unordered_set<unsigned int> indices;

	// 3
	indices = detector.get_indices_in_voxel(glm::vec3(-20,-20,0));
	EXPECT_FALSE(indices.find(0) != indices.end());
	EXPECT_FALSE(indices.find(1) != indices.end());
	EXPECT_FALSE(indices.find(2) != indices.end());
	EXPECT_TRUE(indices.find(3) != indices.end());

	// 0, 3
	indices = detector.get_indices_in_voxel(glm::vec3(-10,-10,0));
	EXPECT_TRUE(indices.find(0) != indices.end());
	EXPECT_FALSE(indices.find(1) != indices.end());
	EXPECT_FALSE(indices.find(2) != indices.end());
	EXPECT_TRUE(indices.find(3) != indices.end());

	// 0, 1, 2
	indices = detector.get_indices_in_voxel(glm::vec3(10,0,0));
	EXPECT_TRUE(indices.find(0) != indices.end());
	EXPECT_TRUE(indices.find(1) != indices.end());
	EXPECT_TRUE(indices.find(2) != indices.end());
	EXPECT_FALSE(indices.find(3) != indices.end());

	// 0, 2
	indices = detector.get_indices_in_voxel(glm::vec3(10,-10,0));
	EXPECT_TRUE(indices.find(0) != indices.end());
	EXPECT_FALSE(indices.find(1) != indices.end());
	EXPECT_TRUE(indices.find(2) != indices.end());
	EXPECT_FALSE(indices.find(3) != indices.end());
}

TEST(VoxelCollisionDetectorTest, ClearTest)
{
	VoxelCollisionDetector detector(10.0f);
	detector.insert(0, glm::vec3(0,0,0));
	detector.insert(1, glm::vec3(20,10,0));
	detector.insert(2, glm::vec3(20,-10,0));
	detector.insert(3, glm::vec3(-20,-20,0));

	detector.clear();
	
	std::unordered_set<unsigned int> indices;

	indices = detector.get_indices_in_voxel(glm::vec3(-20,-20,0));
	EXPECT_FALSE(indices.find(0) != indices.end());
	EXPECT_FALSE(indices.find(1) != indices.end());
	EXPECT_FALSE(indices.find(2) != indices.end());
	EXPECT_FALSE(indices.find(3) != indices.end());

	indices = detector.get_indices_in_voxel(glm::vec3(-10,-10,0));
	EXPECT_FALSE(indices.find(0) != indices.end());
	EXPECT_FALSE(indices.find(1) != indices.end());
	EXPECT_FALSE(indices.find(2) != indices.end());
	EXPECT_FALSE(indices.find(3) != indices.end());

	indices = detector.get_indices_in_voxel(glm::vec3(10,0,0));
	EXPECT_FALSE(indices.find(0) != indices.end());
	EXPECT_FALSE(indices.find(1) != indices.end());
	EXPECT_FALSE(indices.find(2) != indices.end());
	EXPECT_FALSE(indices.find(3) != indices.end());

	indices = detector.get_indices_in_voxel(glm::vec3(10,-10,0));
	EXPECT_FALSE(indices.find(0) != indices.end());
	EXPECT_FALSE(indices.find(1) != indices.end());
	EXPECT_FALSE(indices.find(2) != indices.end());
	EXPECT_FALSE(indices.find(3) != indices.end());
}

TEST(VoxelCollisionDetectorTest, RemoveTest)
{
	VoxelCollisionDetector detector(10.0f);
	detector.insert(0, glm::vec3(0,0,0));
	detector.insert(2, glm::vec3(20,-10,0));
	detector.insert(1, glm::vec3(20,10,0));
	detector.insert(3, glm::vec3(-20,-20,0));

	detector.remove(2);

	std::unordered_set<unsigned int> indices;
	// 0, 1
	indices = detector.get_indices_in_voxel(glm::vec3(10,0,0));
	EXPECT_TRUE(indices.find(0) != indices.end());
	EXPECT_TRUE(indices.find(1) != indices.end());
	EXPECT_FALSE(indices.find(2) != indices.end());
	EXPECT_FALSE(indices.find(3) != indices.end());

	// 0
	indices = detector.get_indices_in_voxel(glm::vec3(10,-10,0));
	EXPECT_TRUE(indices.find(0) != indices.end());
	EXPECT_FALSE(indices.find(1) != indices.end());
	EXPECT_FALSE(indices.find(2) != indices.end());
	EXPECT_FALSE(indices.find(3) != indices.end());
}

