#include "../Objects/InstancedObject3D.h"

#include "../Assets/Model3D.h"
#include "../Assets/ShaderProgram.h"

using CodeMonkeys::Engine::Objects::InstancedObject3D;


InstancedObject3D::InstancedObject3D(Model3D* instanced_model, std::string name, unsigned int instanced_count) :
	Object3D(instanced_model, name),
	instanced_count(instanced_count)
{
	this->instanced_transforms = new mat4[this->instanced_count];
}

void InstancedObject3D::update(float dt)
{

}

void InstancedObject3D::draw(mat4 total_transform, ShaderProgram* shader)
{
	if (this->model != nullptr)
	{
		this->model->draw_instanced(total_transform, shader, this->instanced_transforms, this->instanced_count);
	}
}

void InstancedObject3D::set_instanced_transform(unsigned int instanced_index, glm::mat4 transform)
{
	this->instanced_transforms[instanced_index] = transform;
}
