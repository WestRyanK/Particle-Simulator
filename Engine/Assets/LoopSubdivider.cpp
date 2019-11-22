#include "../Assets/LoopSubdivider.h"

#include "../Assets/ModelLoader.h"
#include <algorithm>
#include <iterator>

using CodeMonkeys::Engine::Assets::LoopSubdivider;
using namespace CodeMonkeys::Engine::Assets;
using namespace glm;
using namespace std;


pair<unsigned int, unsigned int> Triangle::get_edge(unsigned int edge_index)
{
	pair<unsigned int, unsigned int> edge;
	edge.first = this->even_vertex_indices[edge_index % 3];
	edge.second = this->even_vertex_indices[(edge_index + 1) % 3];

	return edge;
}

Triangle::Triangle(const Triangle& triangle)
{
	std::copy(std::begin(triangle.odd_vertex_indices), std::end(triangle.odd_vertex_indices), std::begin(this->odd_vertex_indices));
	std::copy(std::begin(triangle.even_vertex_indices), std::end(triangle.even_vertex_indices), std::begin(this->even_vertex_indices));
	this->index = triangle.index;
}

Vertex::Vertex(const Vertex& vertex)
{
	this->vertex_data = new mlVertex(*vertex.vertex_data);
	this->index = vertex.index;
	this->is_odd = vertex.is_odd;
}

unsigned int Mesh::insert_vertex(Vertex vertex)
{
	unsigned int index = (unsigned int)this->vertices.size();
	vertex.index = index;
	this->vertices.push_back(vertex);
	return index;
}

unsigned int Mesh::insert_triangle(Triangle triangle)
{
	unsigned int index = (unsigned int)this->triangles.size();
	triangle.index = index;
	this->triangles.push_back(triangle);
	return index;
}

set<Triangle> Mesh::get_adjacent_triangles(unsigned int vertex_index)
{
	set<Triangle> adjacent_triangles;
	for (unsigned int i = 0; i < this->triangles.size(); i++)
	{
		for (unsigned int j = 0; j < 3; j++)
		{
			if (this->triangles[i].even_vertex_indices[j] == vertex_index)
			{
				const Triangle t = this->triangles[i];
				adjacent_triangles.insert(t);
			}
		}
	}

	return adjacent_triangles;
}

Mesh::Mesh(const Mesh& mesh)
{
	for (Triangle triangle : mesh.triangles)
	{
		this->insert_triangle(Triangle(triangle));
	}

	for (unsigned int i = 0; i < mesh.vertices.size(); i++)
	{
		this->insert_vertex(Vertex(mesh.vertices[i]));
	}
}

vector<Triangle> Mesh::get_triangles_from_edge(unsigned int vertex_a_index, unsigned int vertex_b_index)
{
	set<Triangle> triangles_a = this->get_adjacent_triangles(vertex_a_index);
	set<Triangle> triangles_b = this->get_adjacent_triangles(vertex_b_index);

	vector<Triangle> common_triangles;
	set_intersection(triangles_a.begin(), triangles_a.end(), triangles_b.begin(), triangles_b.end(), std::back_inserter(common_triangles));

	return common_triangles;
}

int Mesh::get_third_vertex(Triangle triangle, unsigned int vertex_a_index, unsigned int vertex_b_index)
{
	int v = -1;
	for (unsigned int i = 0; i < 3; i++)
	{
		int vertex_index = triangle.even_vertex_indices[i];
		if (!(vertex_index == vertex_a_index || vertex_index == vertex_b_index))
		{
			v = vertex_index;
			break;
		}
	}
	return v;
}

Vertex Vertex::weighted_sum(unsigned int vertex_count, Vertex* vertices, const float* weights)
{
	vec3 position = vec3();
	// vec3 normal;
	vec2 uv = vec2();
	for (unsigned int i = 0; i < vertex_count; i++)
	{
		position += vertices[i].vertex_data->position * weights[i];
		// normal += vertices[i].vertex_data.normal * weights[i];
		uv += vertices[i].vertex_data->UV * weights[i];
	}

	Vertex vertex = Vertex();
	vertex.vertex_data->position = position;
	// vertex.vertex_data.normal = normal;
	vertex.vertex_data->UV = uv;
	return vertex;
}

int Mesh::get_odd_vertex_between_even(Triangle triangle, unsigned int vertex_a_index, unsigned int vertex_b_index)
{
	unsigned int vertices[2] = { vertex_a_index, vertex_b_index };
	unsigned int indices[2] = { 0, 0 };
	for (unsigned int i = 0; i < 2; i++)
	{
		for (unsigned int j = 0; j < 3; j++)
		{
			if (triangle.even_vertex_indices[j] == vertices[i])
				indices[i] = j;
		}
	}

	unsigned int odd_index;
	if (indices[0] > indices[1])
	{
		unsigned int temp = indices[0];
		indices[0] = indices[1];
		indices[1] = temp;
	}

	if (indices[1] == (indices[0] + 1) % 3)
	{
		odd_index = indices[0];
	}
	else
	{
		odd_index = indices[0] + 2;
	}

	int odd_vertex_index = triangle.odd_vertex_indices[odd_index];
	return odd_vertex_index;
}

void LoopSubdivider::add_odd_vertices(Mesh* mesh)
{
	// const float weights[2] = {1.0/2.0f, 1.0/2.0f};
	const float weights[4] = { 3.0f / 8.0f, 3.0f / 8.0f, 1.0f / 8.0f, 1.0f / 8.0f };
	for (unsigned int i = 0; i < mesh->triangles.size(); i++)
	{
		for (unsigned int j = 0; j < 3; j++)
		{
			pair<unsigned int, unsigned int> edge = mesh->triangles[i].get_edge(j);
			vector<Triangle> common_triangles = mesh->get_triangles_from_edge(edge.first, edge.second);


			Triangle other_triangle;
			if (common_triangles[0] == mesh->triangles[i])
				other_triangle = common_triangles[1];
			else
				other_triangle = common_triangles[0];
			int index_of_common_odd_vertex = mesh->get_odd_vertex_between_even(other_triangle, edge.first, edge.second);

			if (index_of_common_odd_vertex == -1)
			{
				Vertex vertices[4];
				vertices[0] = mesh->vertices[edge.first];
				vertices[1] = mesh->vertices[edge.second];
				vertices[2] = mesh->vertices[mesh->get_third_vertex(common_triangles[0], edge.first, edge.second)];
				vertices[3] = mesh->vertices[mesh->get_third_vertex(common_triangles[1], edge.first, edge.second)];

				// Vertex odd_vertex = Vertex::weighted_sum(2, vertices, weights);
				Vertex odd_vertex = Vertex::weighted_sum(4, vertices, weights);
				odd_vertex.is_odd = true;
				unsigned int index = mesh->insert_vertex(odd_vertex);
				mesh->triangles[i].odd_vertex_indices[j] = index;
			}
			else
			{
				mesh->triangles[i].odd_vertex_indices[j] = index_of_common_odd_vertex;
			}
		}
	}
}

Vertex Vertex::adjust_even(float beta, Vertex original_vertex, vector<Vertex> vertices)
{
	vec3 position;
	// vec3 normal;
	vec2 uv;
	for (unsigned int i = 0; i < vertices.size(); i++)
	{
		position += vertices[i].vertex_data->position;
		// normal += vertices[i].vertex_data.normal;
		uv += vertices[i].vertex_data->UV;
	}
	position = position * beta;
	// normal = normal * beta;
	uv = uv * beta;

	position += original_vertex.vertex_data->position * (1 - vertices.size() * beta);
	// normal += original_vertex.vertex_data.normal * (1 - vertices.size() * beta);
	uv += original_vertex.vertex_data->UV * (1 - vertices.size() * beta);

	Vertex vertex;
	vertex.vertex_data->position = position;
	// vertex.vertex_data.normal = normal;
	vertex.vertex_data->UV = uv;
	return vertex;
}

set<Vertex> Mesh::get_adjacent_vertices(Vertex vertex)
{
	set<Triangle> adjacent_triangles = this->get_adjacent_triangles(vertex.index);
	set<Vertex> adjacent_vertices;
	set<Triangle>::iterator it;
	for (it = adjacent_triangles.begin(); it != adjacent_triangles.end(); ++it)
	{
		for (unsigned int j = 0; j < 3; j++)
		{
			const Vertex v = this->vertices[(*it).even_vertex_indices[j]];
			adjacent_vertices.insert(v);
		}
	}

	adjacent_vertices.erase(vertex);
	return adjacent_vertices;
}

void LoopSubdivider::adjust_even_vertices(Mesh* new_mesh, Mesh* old_mesh)
{
	for (unsigned int i = 0; i < old_mesh->vertices.size(); i++)
	{
		Vertex old_vertex = old_mesh->vertices[i];

		set<Vertex> adjacent_vertices_set = old_mesh->get_adjacent_vertices(old_vertex);
		vector<Vertex> adjacent_vertices(adjacent_vertices_set.begin(), adjacent_vertices_set.end());

		float beta;
		if (adjacent_vertices.size() > 3)
			beta = 3.0f / (8.0f * adjacent_vertices.size());
		else
			beta = 3.0f / 16.0f;

		Vertex adjusted_vertex = Vertex::adjust_even(beta, old_vertex, adjacent_vertices);
		adjusted_vertex.index = i;
		adjusted_vertex.is_odd = false;

		new_mesh->vertices[i] = adjusted_vertex;
	}
}

void LoopSubdivider::subdivide_triangles(Mesh* mesh)
{
	vector<Triangle> old_triangles = mesh->triangles;
	vector<Triangle> new_triangles;
	unsigned int triangle_index = 0;
	for (unsigned int i = 0; i < old_triangles.size(); i++)
	{
		for (unsigned int j = 0; j < 3; j++)
		{
			Triangle triangle;
			triangle.even_vertex_indices[0] = old_triangles[i].even_vertex_indices[j];
			triangle.even_vertex_indices[1] = old_triangles[i].odd_vertex_indices[j];
			triangle.even_vertex_indices[2] = old_triangles[i].odd_vertex_indices[(j + 2) % 3];
			triangle.index = triangle_index;

			triangle_index++;
			new_triangles.push_back(triangle);
		}
		Triangle triangle;
		triangle.even_vertex_indices[0] = old_triangles[i].odd_vertex_indices[0];
		triangle.even_vertex_indices[1] = old_triangles[i].odd_vertex_indices[1];
		triangle.even_vertex_indices[2] = old_triangles[i].odd_vertex_indices[2];
		triangle.index = triangle_index;
		triangle_index++;
		new_triangles.push_back(triangle);
	}
	mesh->triangles = new_triangles;
}

vec3 Mesh::get_triangle_normal(Triangle triangle)
{
	vec3 vertices[3];
	for (unsigned int i = 0; i < 3; i++)
	{
		vertices[i] = this->vertices[triangle.even_vertex_indices[i]].vertex_data->position;
	}
	vec3 u = vertices[1] - vertices[0];
	vec3 v = vertices[2] - vertices[0];
	vec3 normal = glm::normalize(glm::cross(u, v));
	return normal;
}

void LoopSubdivider::calculate_normals(Mesh* mesh)
{
	for (unsigned int i = 0; i < mesh->vertices.size(); i++)
	{
		set<Triangle> adjacent_triangles = mesh->get_adjacent_triangles(i);
		vec3 normal = vec3(0.0f);

		set<Triangle>::iterator it;
		for (it = adjacent_triangles.begin(); it != adjacent_triangles.end(); ++it)
		{
			normal += mesh->get_triangle_normal(*it);
		}
		normal = glm::normalize(normal);

		mesh->vertices[i].vertex_data->normal = -normal;
	}
}

mlMesh* LoopSubdivider::calculate_normals(mlMesh* input_mlmesh)
{
	Mesh* mesh = LoopSubdivider::create_mesh_from_mlmesh(input_mlmesh);
	calculate_normals(mesh);
	mlMesh* output_mlmesh = LoopSubdivider::create_mlmesh_from_mesh(mesh);
	free(mesh);

	return output_mlmesh;
}

mlMesh* LoopSubdivider::subdivide_mesh(mlMesh* input_mlmesh, unsigned int subdivide_iterations)
{
	Mesh* mesh = LoopSubdivider::create_mesh_from_mlmesh(input_mlmesh);

	for (unsigned int i = 0; i < subdivide_iterations; i++)
	{

		add_odd_vertices(mesh);
		Mesh* new_mesh = new Mesh(*mesh);
		adjust_even_vertices(new_mesh, mesh);
		subdivide_triangles(new_mesh);

		free(mesh);
		mesh = new_mesh;
	}

	calculate_normals(mesh);
	mlMesh* output_mlmesh = LoopSubdivider::create_mlmesh_from_mesh(mesh);
	free(mesh);

	return output_mlmesh;
}

Mesh* LoopSubdivider::create_mesh_from_mlmesh(mlMesh* input_mesh)
{
	Mesh* mesh = new Mesh();
	for (mlVertex input_vertex : input_mesh->vertices)
	{
		Vertex vertex;
		vertex.vertex_data = new mlVertex(input_vertex);
		mesh->insert_vertex(vertex);
	}

	for (unsigned int i = 0; i < input_mesh->indices.size(); i += 3)
	{
		Triangle triangle;
		for (unsigned int j = 0; j < 3; j++)
		{
			triangle.even_vertex_indices[j] = input_mesh->indices[i + j];
		}
		mesh->insert_triangle(triangle);
	}

	return mesh;
}

mlMesh* LoopSubdivider::create_mlmesh_from_mesh(Mesh* input_mesh)
{
	mlMesh* mesh = new mlMesh();

	for (Vertex input_vertex : input_mesh->vertices)
	{
		mlVertex vertex = mlVertex(*input_vertex.vertex_data);
		mesh->vertices.push_back(vertex);
	}

	for (Triangle triangle : input_mesh->triangles)
	{
		for (unsigned int i = 0; i < 3; i++)
		{
			mesh->indices.push_back(triangle.even_vertex_indices[i]);
		}
	}

	return mesh;
}




mlModel* LoopSubdivider::test()
{
	mlMesh* test_mesh = create_test_mesh();

	mlMesh* subdivided_mesh = LoopSubdivider::subdivide_mesh(test_mesh, 3);
	free(test_mesh);

	mlModel* model = new mlModel();
	// model->meshes.push_back(*test_mesh);
	model->meshes.push_back(subdivided_mesh);

	return model;
}

mlMesh* LoopSubdivider::create_test_mesh()
{
	float scale = 1.0f;
	mlMesh* mesh = new mlMesh();
	vector<mlVertex> vertices;
	mlVertex a;
	a.position = vec3(-scale, -scale, -scale);
	vertices.push_back(a);
	mlVertex b;
	b.position = vec3(scale, -scale, -scale);
	vertices.push_back(b);
	mlVertex c;
	c.position = vec3(scale, -scale, scale);
	vertices.push_back(c);
	mlVertex d;
	d.position = vec3(-scale, -scale, scale);
	vertices.push_back(d);
	mlVertex e;
	e.position = vec3(-scale, scale, -scale);
	vertices.push_back(e);
	mlVertex f;
	f.position = vec3(scale, scale, -scale);
	vertices.push_back(f);
	mlVertex g;
	g.position = vec3(scale, scale, scale);
	vertices.push_back(g);
	mlVertex h;
	h.position = vec3(-scale, scale, scale);
	vertices.push_back(h);
	vector<unsigned int> indices = { 0, 1, 3, 1, 2, 3, 4, 5, 7, 5, 6, 7, 0, 1, 4, 1, 5, 4, 1, 2, 5, 2, 6, 5, 2, 3, 6, 3, 7, 6, 3, 0, 7, 0, 4, 7 };

	// float SCALE = 5.0f;
	// float one_over_sqrt_2 = 1/sqrt(2.0);
	// mlMesh* mesh = new mlMesh();
	// vector<mlVertex> vertices;
	// mlVertex a;
	// a.position = vec3(SCALE * 1.0f, 0.0f, SCALE * -one_over_sqrt_2);
	// vertices.push_back(a);
	// mlVertex b;
	// b.position = vec3(SCALE * -1.0f, 0.0f, SCALE * -one_over_sqrt_2);
	// vertices.push_back(b);
	// mlVertex c;
	// c.position = vec3(0.0f, SCALE * 1.0f, SCALE * one_over_sqrt_2);
	// vertices.push_back(c);
	// mlVertex d;
	// d.position = vec3(0.0f, SCALE * -1.0f, SCALE * one_over_sqrt_2);
	// vertices.push_back(d);
	// vector<unsigned int> indices = { 0, 1, 2, 1, 2, 3, 0, 2, 3, 0, 1, 3 };
	mesh->vertices = vertices;
	mesh->indices = indices;
	return mesh;
}