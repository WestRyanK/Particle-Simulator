#pragma once

#include "../Engine/Dependencies/OpenGL.h"
#include "../Engine/Engine/GameEngine.h"

using namespace CodeMonkeys::Engine::Engine;

namespace CodeMonkeys::GranularSimulator
{
    class GranularSimulatorEngine: public GameEngine
    {
    private:
        void update_frame(float dt);
		void init_simulator(ShaderProgram* shader);

    public:
        GranularSimulatorEngine(GLFWwindow* window, GLuint width, GLuint height);
        void init();
    };

}