#ifndef _MODEL_LOADER_H_
#define _MODEL_LOADER_H_

#include "../Dependencies/OpenGL.h"
#include <string>
#include <vector>

using namespace glm;

namespace CodeMonkeys::Engine::Assets
{
	struct mlVertex{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 UV;
		mlVertex() : position(glm::vec3()), normal(glm::vec3()), UV(glm::vec2()) { };
		mlVertex(const mlVertex& vertex) { position = vec3(vertex.position); normal = vec3(vertex.normal); UV = vec2(vertex.UV); };
	};

	inline bool operator==(const mlVertex& lhs, const mlVertex& rhs)
	{
		return lhs.position == rhs.position &&
			lhs.normal      == rhs.normal &&
			lhs.UV          == rhs.UV;
	};

	struct mlMesh {
		std::string name;
		std::string textureFile;
		std::vector<mlVertex> vertices;
		std::vector<GLuint> indices;
	};

	// mlModel contains all the loaded data for one model file.
	// One model can contain several meshes. This allows each mesh
	// to have its own texture.
	struct mlModel {
		std::vector<mlMesh*> meshes;
	};

	// Attempts to load the 3d model contained in modelFile. Fills out the modelOut
	// struct and returns true on success. Returns false and prints errors on failure.
	bool LoadModel(const std::string& directory, const std::string& modelFile, mlModel& modelOut);
}

#endif