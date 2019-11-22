#ifndef _INTERSECT_DETECTOR_H_
#define _INTERSECT_DETECTOR_H_

namespace CodeMonkeys::Engine::Collision
{
	class ICollisionRegion;
	class BoundingSphere;

    class IntersectDetector
    {
    public:
        static bool is_intersection(ICollisionRegion* region_a, ICollisionRegion* region_b);
        // static bool is_intersection(BoundingBox* region_a, BoundingBox* region_b);
        static bool is_intersection(BoundingSphere* region_a, BoundingSphere* region_b);
        // static bool is_intersection(BoundingBox* region_a, BoundingSphere* region_b);
        // static bool is_intersection(BoundingSphere* region_a, BoundingBox* region_b);
    };
}

#endif