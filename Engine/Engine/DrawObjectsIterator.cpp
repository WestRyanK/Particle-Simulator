#include "DrawObjectsIterator.h"

#include "../Assets/ShaderProgram.h"
#include "../Objects/Object3D.h"

using CodeMonkeys::Engine::Engine::DrawObjectsIterator;


void DrawObjectsIterator::visit_object(Object3D* object)
{
	mat4 transform = object->get_transform();
	if (!this->transform_stack.empty())
	{
		mat4 parent_transform = this->transform_stack.top();
		transform = parent_transform * transform;
	}
	this->transform_stack.push(transform);

	object->draw(transform, current_shader);
}

void DrawObjectsIterator::post_visit_children(Object3D* parent)
{
	if (!this->transform_stack.empty())
	{
		this->transform_stack.pop();
	}
}

void DrawObjectsIterator::draw(Object3D* world_root, ShaderProgram* current_shader)
{
	this->current_shader = current_shader;
	this->traverse(world_root);
}