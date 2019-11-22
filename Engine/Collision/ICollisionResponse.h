#ifndef _ICOLLISION_RESPONSE_H_
#define _ICOLLISION_RESPONSE_H_

namespace CodeMonkeys::Engine
{ 
	namespace Engine 
	{ 
		class GameEngine; 
	}
	namespace Objects
	{
		class Object3D;
	}
}

using namespace CodeMonkeys::Engine::Engine;
using namespace CodeMonkeys::Engine::Objects;

namespace CodeMonkeys::Engine::Collision
{
    class ICollisionResponse
    {
    protected:
        GameEngine* engine;
    public:
        ICollisionResponse(GameEngine* engine);
        virtual bool can_respond(Object3D* object_a, Object3D* object_b) = 0;
        virtual void respond(Object3D* object_a, Object3D* object_b, float dt) = 0;
    };
}

#endif
