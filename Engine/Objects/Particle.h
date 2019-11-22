#ifndef _PARTICLE_H_
#define _PARTICLE_H_

#include "../Objects/PhysicalObject3D.h"

namespace CodeMonkeys::Engine
{
	namespace Assets
	{
		class Model3D;
	}
	namespace Collision
	{
		class BoundingMultiSphere;
	}
	namespace Objects 
	{ 
		class ParticleEmitter; 
		class Billboard;
	}
}

using namespace CodeMonkeys::Engine::Assets;
using namespace CodeMonkeys::Engine::Collision;
using namespace CodeMonkeys::Engine::Objects;

namespace CodeMonkeys::Engine::Objects
{
    class Particle : public PhysicalObject3D
    {
    protected:
        float total_lifespan;
        float current_lifespan;
        ParticleEmitter* emitter = NULL;
        BoundingMultiSphere* collision_region = NULL;
        Billboard* billboard = NULL;
    public:
        Particle(Model3D* model, Billboard* billboard, string name, float total_lifespan, ParticleEmitter* emitter);
        ~Particle();
        void kill();
        virtual Particle* clone();
        virtual void update(float dt);
        ParticleEmitter* get_emitter();
        virtual void draw(mat4 total_transform, ShaderProgram* shader);
        void set_collision_region(BoundingMultiSphere* collision_region);
        virtual ICollisionRegion* get_collision_region();
    };
}

#endif