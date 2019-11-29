#ifndef _MODEL_3D_H_
#define _MODEL_3D_H_

#include "../Dependencies/OpenGL.h"
#include <string>
#include <vector>

namespace CodeMonkeys::Engine
{
	namespace Assets
	{
		struct mlModel;
		struct mlMesh;
		class Material;
		class ShaderProgram;
	}
}

using namespace CodeMonkeys::Engine::Assets;
using namespace glm;
using namespace std;


namespace CodeMonkeys::Engine::Assets
{
    // Represents a 3D model with its textures and shaders.
    class Model3D
    {
    private:
		
        const static int POSITION_INDEX = 0;
        const static int NORMAL_INDEX = 1;
        const static int UV_INDEX = 2;
        const static int INSTANCED_MAT_INDEX_1 = 3;
        const static int INSTANCED_MAT_INDEX_2 = 4;
        const static int INSTANCED_MAT_INDEX_3 = 5;
        const static int INSTANCED_MAT_INDEX_4 = 6;

        const static unsigned int POSITION_SIZE = 3;
        const static unsigned int NORMAL_SIZE = 3;
        const static unsigned int UV_SIZE = 2;
        const static unsigned int VEC4_SIZE = sizeof(glm::vec4);

        vector<unsigned int> ebos;
        vector<unsigned int> ebo_sizes;
        vector<VAO> vaos;
        mlModel* ml_model = nullptr;
        vector<Material*> materials;

        void create_vao_ebo(mlMesh* mesh);

    public:
        Model3D(mlModel* ml_model, vector<Material*> material);
        Model3D* clone();
        // Render the Model3D using its model, textures, and shaders.
        void draw(mat4 transform, ShaderProgram* shader);
		void draw_instanced(mat4 transform, ShaderProgram* shader, mat4* instanced_transforms, unsigned int count);
        void update_materials(float dt);
    };
};

#endif