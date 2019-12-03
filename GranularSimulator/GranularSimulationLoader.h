#ifndef _GRANULAR_SIMULATION_LOADER_H_
#define _GRANULAR_SIMULATION_LOADER_H_

#include <string>

namespace CodeMonkeys::GranularSimulator
{
	class GranularSubstanceSimulator;
}

namespace CodeMonkeys::GranularSimulator
{
	class GranularSimulationLoader
	{
	public:
		static void save_simulation(std::string filename, GranularSubstanceSimulator* simulator);
		static GranularSubstanceSimulator* load_simuation(std::string filename);
	};
}

#endif
