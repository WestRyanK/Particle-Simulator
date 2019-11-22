#ifndef _LOOP_SUBDIVIDER_H_
#define _LOOP_SUBDIVIDER_H_

#include "../Dependencies/OpenGL.h"
#include <set>
#include <utility>
#include <vector>

namespace CodeMonkeys::Engine
{
	namespace Assets
	{
		struct mlMesh;
		struct mlModel;
		struct mlVertex;
	}
}

using namespace CodeMonkeys::Engine::Assets;
using namespace glm;
using namespace std;


namespace CodeMonkeys::Engine::Assets
{
    class Triangle
    {
    public:
        int index = -1;
        int even_vertex_indices[3] = { -1, -1, -1};
        int odd_vertex_indices[3] = { -1, -1, -1};
        pair<unsigned int, unsigned int> get_edge(unsigned int edge_index);
        
        Triangle() { };
        Triangle(const Triangle& triangle);


        bool operator<(const Triangle& rhs) const 
        {
            return this->index < rhs.index;
        }

        bool operator==(const Triangle& rhs) const
        {
            return this->index == rhs.index;
        }
    };

    class Vertex
    {
    public:
        int index = -1;
        bool is_odd = false;
        mlVertex* vertex_data = nullptr;
        Vertex() { };
        Vertex(const Vertex& vertex);
        static Vertex weighted_sum(unsigned int vertex_count, Vertex* vertices, const float* weights);
        static Vertex adjust_even(float beta, Vertex original_vertex, vector<Vertex> vertices);

        bool operator<(const Vertex& rhs) const 
        {
            return this->index < rhs.index;
        }

        bool operator==(const Triangle& rhs) const
        {
            return this->index == rhs.index;
        }
    };

    class Mesh
    {
    public:
        vector<Vertex> vertices;
        vector<Triangle> triangles;
        Mesh() { };
        Mesh(const Mesh& mesh);
        unsigned int insert_vertex(Vertex vertex);
        unsigned int insert_triangle(Triangle triangle);
        int get_third_vertex(Triangle triangle, unsigned int vertex_a_index, unsigned int vertex_b_index);
        vector<Triangle> get_triangles_from_edge(unsigned int vertex_a_index, unsigned int vertex_b_index);
        set<Triangle> get_adjacent_triangles(unsigned int vertex_index);
        set<Vertex> get_adjacent_vertices(Vertex vertex);
        int get_odd_vertex_between_even(Triangle triangle, unsigned int vertex_a_index, unsigned int vertex_b_index);
        vec3 get_triangle_normal(Triangle triangle);
    };

    class LoopSubdivider
    {
    private:
        static Mesh* create_mesh_from_mlmesh(mlMesh* input_mlmesh);
        static mlMesh* create_mlmesh_from_mesh(Mesh* input_mesh);
        static void add_odd_vertices(Mesh* mesh);
        static void adjust_even_vertices(Mesh* new_mesh, Mesh* old_mesh);
        static void subdivide_triangles(Mesh* mesh);
        static void calculate_normals(Mesh* mesh);
    public:
        static mlMesh* subdivide_mesh(mlMesh* input_mlmesh, unsigned int subdivide_iterations);
        static mlModel* test();
        static mlMesh* calculate_normals(mlMesh* input_mlmesh);
        static mlMesh* create_test_mesh();
    };
}

#endif
