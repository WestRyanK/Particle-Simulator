#pragma once

#include "../Collision/ICollisionRegion.h"
#include "../Dependencies/OpenGL.h"

using namespace glm;

namespace CodeMonkeys::Engine::Collision
{
    class BoundingBox : public ICollisionRegion
    {
    private:
        // The corner in the +X, +Y, +Z direction.
        vec3 corner_max;
        // The corner in the -X, -Y, -Z direction.
        vec3 corner_min;
    public:
        BoundingBox(vec3 corner_max, vec3 corner_min);
        vec3 get_corner_max();
        void set_corner_max(vec3 corner_max);
        vec3 get_corner_min();
        void set_corner_min(vec3 corner_min);

        float get_min_x();
        float get_min_y();
        float get_min_z();
        float get_max_x();
        float get_max_y();
        float get_max_z();
        void set_min_x(float x);
        void set_min_y(float y);
        void set_min_z(float z);
        void set_max_x(float x);
        void set_max_y(float y);
        void set_max_z(float z);
    };
}