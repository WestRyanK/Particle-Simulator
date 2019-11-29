#ifndef _INSTANCED_OBJECT_3D_H_
#define _INSTANCED_OBJECT_3D_H_

#include "../Objects/Object3D.h"

namespace CodeMonkeys::Engine::Objects
{
	class InstancedObject3D : public Object3D
	{
	private:
		glm::mat4* instanced_transforms;
		unsigned int instanced_count;

	public:
		InstancedObject3D(Model3D* instanced_model, std::string name, unsigned int instanced_count);

		virtual void update(float dt);
		virtual void draw(mat4 total_transform, ShaderProgram* shader);
		void set_instanced_transform(unsigned int instanced_index, glm::mat4 transform);
	};
}
#endif
