
#include "pch.h"
#include "../GranularSimulator/BodyParticleGenerator.cpp"
#include "../GranularSimulator/BodyParticleGenerator.h"
//#include "../GranularSimulator/VoxelCollisionDetector.h"
//#include "../GranularSimulator/VoxelCollisionDetector.cpp"
#include <glm/glm.hpp>
#include <vector>

//using namespace CodeMonkeys::GranularSimulator;

TEST(BodyParticleGeneratorTest, CalculateInertialMomentTest)
{
	std::vector<glm::vec3> offsets;
	offsets.push_back(glm::vec3(-1.f, 1.f, 1.f));

	std::vector<float> sizes;
	sizes.push_back(2.0f);
	
	glm::vec3 center = glm::vec3(0.f);
	glm::mat3 inertial_moment1 = BodyParticleGenerator::calculate_inertial_moment(offsets, center, 1.f, sizes);


	glm::mat3 expected_inertial_moment1 = glm::transpose(glm::make_mat3(new float[9]{ 4.f, 2.f, 2.f, 2.f, 4.f, -2.f, 2.f, -2.f, 4.f }));

	EXPECT_EQ(expected_inertial_moment1, inertial_moment1);

	offsets.clear();
	offsets.push_back(glm::vec3(2.f, 3.f, 4.f));
	offsets.push_back(glm::vec3(5.f, 6.f, 7.f));
	center = glm::vec3(1.f, 0.f, -1.f);

	sizes.push_back(2.0f);

	glm::mat3 inertial_moment2 = BodyParticleGenerator::calculate_inertial_moment(offsets, center, 1.f, sizes);
	glm::mat3 expected_inertial_moment2 = glm::transpose(glm::make_mat3(new float[9]{268, -54, -74, -54, 212, -126, -74, -126, 124}));

	EXPECT_EQ(expected_inertial_moment2, inertial_moment2);
	

	// m = 2;
	// Ixx = Sum(m * ((yi - y)^2 + (zi - z)^2)) = (2 * ((3 - 0)^2 + (4 - -1)^2) + (2 * ((6 - 0)^2 + (7 - -1)^2) = 268
	// Iyy = Sum(m * ((xi - x)^2 + (zi - z)^2)) = (2 * ((2 - 1)^2 + (4 - -1)^2) + (2 * ((5 - 1)^2 + (7 - -1)^2) = 212
	// Izz = Sum(m * ((xi - x)^2 + (yi - y)^2)) = (2 * ((2 - 1)^2 + (3 - 0)^2) + (2 * ((5 - 1)^2 + (6 - 0)^2) = 124
	// Iyz = Sum( -m * (yi - y) * (zi - z)) = (-2 * (3 - 0) * (4 - -1)) + (-2 * (6 - 0) * (7 - -1)) = -126
	// Ixz = Sum( -m * (xi - x) * (zi - z)) = (-2 * (2 - 1) * (4 - -1)) + (-2 * (5 - 1) * (7 - -1)) = -74
	// Ixy = Sum( -m * (xi - x) * (yi - y)) = (-2 * (2 - 1) * (3 - 0)) + (-2 * (5 - 1) * (6 - 0)) = -54
} 

TEST(BodyParticleGeneratorTest, MatrixTest)
{
	glm::mat4 mat = glm::transpose(glm::make_mat4(new float[16]{ 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f, 11.f, 12.f, 13.f, 14.f, 15.f, 16.f}));

	glm::mat3 mat2 = mat;

	glm::mat3 expected_mat2 = glm::transpose(glm::make_mat3(new float[9]{ 1.f, 2.f, 3.f, 5.f, 6.f, 7.f, 9.f, 10.f, 11.f}));
	EXPECT_EQ(expected_mat2, mat2);
}
