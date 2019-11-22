// GranularSimulator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "GranularSubstance.h"
#include "GranularSimulatorWindow.h"
#include "GranularSimulatorEngine.h"
#include "RandomGenerator.h"

using namespace CodeMonkeys::GranularSimulator;

int main()
{
	RandomGenerator::Init();
	int width = 640;
	int height = 480;
	GranularSimulatorWindow window = GranularSimulatorWindow(width, height, false);
	GranularSimulatorEngine engine = GranularSimulatorEngine(window.get_window(), width, height);

	engine.init();
	engine.run();
}