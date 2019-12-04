#include "../Assets/Model3D.h"

#include "../Assets/Material.h"
#include "../Assets/ModelLoader.h"
#include "../Assets/ShaderProgram.h"

using CodeMonkeys::Engine::Assets::Model3D;
using namespace CodeMonkeys::Engine::Assets;
using namespace glm;


Model3D::Model3D(mlModel* ml_model, vector<Material*> materials)
{
	if (ml_model != NULL)
	{
		for (unsigned int i = 0; i < ml_model->meshes.size(); i++)
		{
			this->create_vao_ebo(ml_model->meshes[i]);
		}
	}

	this->ml_model = ml_model;
	for (unsigned int i = 0; i < materials.size(); i++)
	{
		this->materials.push_back(materials[i]);
	}
}

void Model3D::create_vao_ebo(mlMesh* mesh)
{
	VAO vao;
	VBO vbo;
	EBO ebo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, mesh->vertices.size() * sizeof(mlVertex), &(mesh->vertices[0]), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indices.size() * sizeof(unsigned int), &(mesh->indices[0]), GL_STATIC_DRAW);

	glEnableVertexAttribArray(POSITION_INDEX);
	glVertexAttribPointer(POSITION_INDEX, POSITION_SIZE, GL_FLOAT, GL_FALSE, sizeof(mlVertex), (void*)0);

	glEnableVertexAttribArray(NORMAL_INDEX);
	glVertexAttribPointer(NORMAL_INDEX, NORMAL_SIZE, GL_FLOAT, GL_FALSE, sizeof(mlVertex), (void*)offsetof(mlVertex, normal));

	glEnableVertexAttribArray(UV_INDEX);
	glVertexAttribPointer(UV_INDEX, UV_SIZE, GL_FLOAT, GL_FALSE, sizeof(mlVertex), (void*)offsetof(mlVertex, UV));

	glBindVertexArray(0);
	this->vaos.push_back(vao);
	this->ebos.push_back(vao);
	this->ebo_sizes.push_back((unsigned int)mesh->indices.size());
}

void Model3D::draw(mat4 transform, ShaderProgram* shader)
{
	for (unsigned int i = 0; i < this->vaos.size(); i++)
	{
		if (this->materials[i]->get_shader() == shader)
		{
			this->materials[i]->get_shader()->setUniform("is_instanced", false);
			this->materials[i]->get_shader()->setUniform("object_transform", transform);
			this->materials[i]->apply_material_to_shader();

			glBindVertexArray(this->vaos[i]);
			glDrawElements(GL_TRIANGLES, this->ebo_sizes[i], GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
	}
}

void Model3D::draw_instanced(mat4 transform, ShaderProgram* shader, mat4* instanced_transforms, unsigned int count)
{
	unsigned int instanced_buffer;
	glGenBuffers(1, &instanced_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, instanced_buffer);
	glBufferData(GL_ARRAY_BUFFER, count * sizeof(glm::mat4), &instanced_transforms[0], GL_DYNAMIC_DRAW);

	for (unsigned int i = 0; i < this->vaos.size(); i++)
	{
		if (this->materials[i]->get_shader() == shader)
		{
			this->materials[i]->get_shader()->setUniform("is_instanced", true);
			this->materials[i]->get_shader()->setUniform("object_transform", transform);
			this->materials[i]->apply_material_to_shader();

			glBindVertexArray(this->vaos[i]);
			
			const GLsizei vec4_size = sizeof(glm::vec4);
			glEnableVertexAttribArray(INSTANCED_MAT_INDEX_1);
			glVertexAttribPointer(INSTANCED_MAT_INDEX_1, 4, GL_FLOAT, GL_FALSE, 4 * VEC4_SIZE, (void*)(0 * VEC4_SIZE));
			glEnableVertexAttribArray(INSTANCED_MAT_INDEX_2);
			glVertexAttribPointer(INSTANCED_MAT_INDEX_2, 4, GL_FLOAT, GL_FALSE, 4 * VEC4_SIZE, (void*)(1 * VEC4_SIZE));
			glEnableVertexAttribArray(INSTANCED_MAT_INDEX_3);
			glVertexAttribPointer(INSTANCED_MAT_INDEX_3, 4, GL_FLOAT, GL_FALSE, 4 * VEC4_SIZE, (void*)(2 * VEC4_SIZE));
			glEnableVertexAttribArray(INSTANCED_MAT_INDEX_4);
			glVertexAttribPointer(INSTANCED_MAT_INDEX_4, 4, GL_FLOAT, GL_FALSE, 4 * VEC4_SIZE, (void*)(3 * VEC4_SIZE));

			glVertexAttribDivisor(INSTANCED_MAT_INDEX_1, 1);
			glVertexAttribDivisor(INSTANCED_MAT_INDEX_2, 1);
			glVertexAttribDivisor(INSTANCED_MAT_INDEX_3, 1);
			glVertexAttribDivisor(INSTANCED_MAT_INDEX_4, 1);

			glDrawElementsInstanced(GL_TRIANGLES, this->ebo_sizes[i], GL_UNSIGNED_INT, 0, count);
			glBindVertexArray(0);
		}
	}

	glDeleteBuffers(1, &instanced_buffer);
}

Model3D* Model3D::clone()
{
	vector<Material*> materials_clone;
	for (unsigned int i = 0; i < this->materials.size(); i++)
	{
		Material* material_clone = this->materials[i]->clone();
		materials_clone.push_back(material_clone);
	}
	Model3D* model_clone = new Model3D(NULL, materials_clone);
	model_clone->vaos = this->vaos;
	model_clone->ml_model = this->ml_model;
	model_clone->ebos = this->ebos;
	model_clone->ebo_sizes = this->ebo_sizes;
	return model_clone;
}

void Model3D::update_materials(float dt)
{
	for (unsigned int i = 0; i < this->materials.size(); i++)
	{
		this->materials[i]->update(dt);
	}
}
