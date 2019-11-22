#include "../Assets/ModelLoader.h"

#include <iostream>

namespace CodeMonkeys::Engine::Assets
{
	glm::vec3 AiVec3ToGlm3(aiVector3D v) {
		glm::vec3 res;
		res.x = v.x;
		res.y = v.y;
		res.z = v.z;
		return res;
	}

	glm::vec2 AiVec3ToGlm2(aiVector3D v) {
		glm::vec2 res;
		res.x = v.x;
		res.y = v.y;
		return res;
	}

	static void ProcessNode(aiNode* node, const aiScene* scene, const std::string& directory, mlModel& modelOut) {
		for (unsigned int meshIdx = 0; meshIdx < node->mNumMeshes; meshIdx++) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[meshIdx]];

			mlMesh* ml_mesh = new mlMesh();
		
			ml_mesh->name = std::string(mesh->mName.C_Str());
			for (unsigned int vIdx = 0; vIdx < mesh->mNumVertices; vIdx++) {
				mlVertex vertex;
				vertex.position = AiVec3ToGlm3(mesh->mVertices[vIdx]);
				vertex.normal = AiVec3ToGlm3(mesh->mNormals[vIdx]);

				if (mesh->mTextureCoords[0]) {
					vertex.UV = AiVec3ToGlm2(mesh->mTextureCoords[0][vIdx]);
				}

				ml_mesh->vertices.push_back(vertex);
			}

			for (unsigned int fIdx = 0; fIdx < mesh->mNumFaces; fIdx++) {
				aiFace face = mesh->mFaces[fIdx];
				for (unsigned int iIdx = 0; iIdx < face.mNumIndices; iIdx++) {
					ml_mesh->indices.push_back(face.mIndices[iIdx]);
				}
			}

			// Right now this just supports a single diffuse texture per mesh.
			if (mesh->mMaterialIndex >= 0) {
				aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
				aiString str;
				mat->GetTexture(aiTextureType_DIFFUSE, 0, &str);
				std::string tex(str.C_Str());
				ml_mesh->textureFile = directory + tex;
			}

			modelOut.meshes.push_back(ml_mesh);
		}

		for (unsigned int cIdx = 0; cIdx < node->mNumChildren; cIdx++) {
			ProcessNode(node->mChildren[cIdx], scene, directory, modelOut);
		}
	}

	bool LoadModel(const std::string& directory, const std::string& modelFile, mlModel& modelOut) {
		std::string dir = directory;
		std::string full;
		if (dir[dir.length() - 1] == '/' || dir[dir.length() - 1] == '\\') {
			full = dir + modelFile;
		}
		else {
			dir += '/';
			full = dir + modelFile;
		}

		Assimp::Importer imp;
		const aiScene* scene = imp.ReadFile(full,
			aiProcess_GenNormals |
			aiProcess_Triangulate |
			aiProcess_FlipUVs);
		
		if (!scene || !scene->mRootNode) {
			std::cout << imp.GetErrorString() << std::endl;
			return false;
		}

		ProcessNode(scene->mRootNode, scene, dir, modelOut);
		if (modelOut.meshes.size() == 0) {
			std::cout << "Model file contained no meshes." << std::endl;
			return false;
		}

		return true;
	}
}
