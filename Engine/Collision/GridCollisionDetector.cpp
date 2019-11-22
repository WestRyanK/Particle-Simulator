#include "../Collision/GridCollisionDetector.h"

#include "../Collision/IntersectDetector.h"
#include "../Objects/Object3D.h"

using CodeMonkeys::Engine::Collision::GridCollisionDetector;


GridCollisionDetector::GridCollisionDetector(vec3 positive_bounds, vec3 negative_bounds, float grid_size) : ICollisionDetector(positive_bounds, negative_bounds)
{
    this->grid_size = grid_size;

    this->create_grid();
}

void GridCollisionDetector::create_grid()
{
    float x_size = this->get_positive_bounds().x - this->get_negative_bounds().x;
    float y_size = this->get_positive_bounds().y - this->get_negative_bounds().y;
    float z_size = this->get_positive_bounds().z - this->get_negative_bounds().z;

    this->x_count = (unsigned int)ceil(x_size / this->grid_size);
    this->y_count = (unsigned int)ceil(y_size / this->grid_size);
    this->z_count = (unsigned int)ceil(z_size / this->grid_size);

    this->grid = new set<Object3D*>*[this->x_count * this->y_count * this->z_count] { 0 };
}

ivec3 GridCollisionDetector::get_grid_cell_location(vec3 position)
{
    vec3 delta_position = position - this->get_negative_bounds();
    int x = (int) (delta_position.x / this->grid_size);
    int y = (int) (delta_position.y / this->grid_size);
    int z = (int) (delta_position.z / this->grid_size);

    return ivec3(x, y, z);
}

set<Object3D*>* GridCollisionDetector::get_grid_cell(ivec3 location)
{
    if ((unsigned int)location.x >= this->x_count || location.x < 0
     || (unsigned int)location.y >= this->y_count || location.y < 0
     || (unsigned int)location.z >= this->z_count || location.z < 0)
    {
        return NULL;
    }
    else
    {
        int index = location.z * (this->x_count * this->y_count) + location.y * (this->x_count) + location.x;
        auto cell = this->grid[index];
        if (cell == NULL)
        {
            cell = new set<Object3D*>();
            this->grid[index] = cell;
        }
        return cell;
    }
}

void GridCollisionDetector::clear()
{
    free(this->grid);
    this->create_grid();
}

void GridCollisionDetector::insert(Object3D* object)
{
    // If the object we're adding doesn't have collision regions, there is no reason to add it!
    if (object != NULL && object->get_collision_region() != NULL)
    {
        vec3 position = object->get_transformed_position();
        ivec3 location = this->get_grid_cell_location(position);
        auto cell = this->get_grid_cell(location);
        if (cell != NULL)
        {
            cell->insert(object);

            this->object_location_lookup[object] = location;
        }
        else
        {
            // printf("Out of bounds!");
        }
    }
}

void GridCollisionDetector::remove(Object3D* object)
{
    auto location = this->object_location_lookup.find(object);
    this->object_location_lookup.erase(object);
    if (location != this->object_location_lookup.end())
    {
        auto cell = this->get_grid_cell((*location).second);
        cell->erase(object);
    }
}

void GridCollisionDetector::update(Object3D* object)
{
    // If an object was removed from the world, its parent will be null.
    // Therefore, if parent is null, remove it from the collision detector.
    if (object != NULL && object->get_parent() != NULL)
    {
        vec3 position = object->get_transformed_position();
        ivec3 location = this->get_grid_cell_location(position);

        auto previous_location = this->object_location_lookup.find(object);
        if (previous_location == this->object_location_lookup.end())
        {
            this->insert(object);
        }
        else if (location != (*previous_location).second)
        {
            this->remove(object);
            this->insert(object);
        }
    }
    else
    {
        this->remove(object);
    }
}

set<pair<Object3D*, Object3D*>> GridCollisionDetector::get_collisions()
{
    set<pair<Object3D*, Object3D*>> collisions;

    for (unsigned int z = 0; z < this->z_count; z++)
    {
        for (unsigned int y = 0; y < this->y_count; y++)
        {
            for (unsigned int x = 0; x < this->x_count; x++)
            {
                auto current_cell = this->get_grid_cell(ivec3(x, y, z));
                if (current_cell != NULL)
                {
                    for (unsigned int z_off = 0; z_off <= 1; z_off++)
                    {
                        for (unsigned int y_off = 0; y_off <= 1; y_off++)
                        {
                            for (unsigned int x_off = 0; x_off <= 1; x_off++)
                            {
                                auto other_cell = this->get_grid_cell(ivec3(x + x_off, y + y_off, z + z_off));
                                if (other_cell != NULL)
                                {
                                    ICollisionDetector::get_collisions_among_objects(&collisions, current_cell, other_cell);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return collisions;
}