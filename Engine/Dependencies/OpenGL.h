#ifndef _OPENGL_H_
#define _OPENGL_H_

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "stb/stb_image.h"


#pragma warning(push)
#include "assimp/vector3.h"
#include "assimp/scene.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#pragma warning(pop)

using namespace glm;

typedef unsigned int VAO;
typedef unsigned int VBO;
typedef unsigned int EBO;

#endif
