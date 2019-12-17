#pragma once

#include "../Engine/Dependencies/OpenGL.h"
#include "../Engine/Engine/GameEngine.h"
#include <string>
#include <vector>

using namespace CodeMonkeys::Engine::Engine;

namespace CodeMonkeys::GranularSimulator
{
    class GranularSimulatorEngine: public GameEngine
    {
    private:
        void update_frame(float dt);
		void init_simulator(ShaderProgram* shader);
		std::vector<std::string> settings;

    public:
        GranularSimulatorEngine(GLFWwindow* window, GLuint width, GLuint height, std::vector<std::string> settings);
        void init();
    };

}