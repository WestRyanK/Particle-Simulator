// GranularSimulator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "GranularSimulatorEngine.h"
#include "GranularSimulatorWindow.h"
#include "GranularSubstance.h"
#include "RandomGenerator.h"
#include <iostream>
#include <string>
#include <vector>

using namespace CodeMonkeys::GranularSimulator;

std::vector<std::string> parse_settings(int argc, char* argv[])
{
	std::vector<std::string> settings;
	for (unsigned int i = 0; i < argc; i++)
	{
		std::string arg = std::string(argv[i]);
		settings.push_back(arg);
	}

	return settings;
}

int main(int argc, char* argv[])
{
	std::vector<std::string> settings = parse_settings(argc, argv);

	RandomGenerator::Init();
	int width = 1920;
	int height = 1080;
	GranularSimulatorWindow window = GranularSimulatorWindow(width, height, false);
	GranularSimulatorEngine engine = GranularSimulatorEngine(window.get_window(), width, height, settings);

	engine.init();
	engine.run();
}
