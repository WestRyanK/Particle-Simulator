#include "../Collision/ICollisionResponse.h"

#include "../Objects/Object3D.h"
#include "../Engine/GameEngine.h"

using CodeMonkeys::Engine::Collision::ICollisionResponse;
using namespace CodeMonkeys::Engine::Engine;


ICollisionResponse::ICollisionResponse(GameEngine* engine)
{
	this->engine = engine;
}