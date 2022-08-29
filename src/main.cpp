#include <getopt.h>
#include <plog/Initializers/RollingFileInitializer.h>
#include <plog/Log.h>

#include <chrono>
#include <cstring>
#include <fstream>
#include <iostream>

#include "circuit_simulator.h"
#include "pcl_test.h"

void Usage(const std::string& prog_name) {
	std::cout << prog_name << " [Options]" << std::endl
	          << "Options:" << std::endl
	          << "-h	Print this message" << std::endl
	          << "-c	Path to circuit file" << std::endl
	          << "-o	Computer the concave hull of the specified pcd file" << std::endl;
}

int main(int argc, char* argv[]) {
	auto program_start = std::chrono::high_resolution_clock::now();
	std::string file_name;
	bool compute_concave_hull = false;
	while (true) {
		switch (getopt(argc, argv, "o:c:h")) {
		case 'o':
			compute_concave_hull = true;
		case 'c':
			file_name = std::string(optarg);
			break;
		case '?':
		case 'h':
		case -1:
		default:
			Usage(std::string(argv[0]));
			return 0;
		}
		break;
	}

	if (compute_concave_hull) {
		PCLTest pcl_test;
		pcl_test.ComputeConcaveHull(file_name);
		return 0;
	}

	plog::init(plog::debug, "efficient_mis_modelling.log");
	CircuitSimulator simulator = CircuitSimulator();
	try {
		simulator.InitializeCircuit(file_name);
	} catch (const std::exception& e) {
		return -1;
	}

	auto simulation_start = std::chrono::high_resolution_clock::now();

	simulator.SimulateCircuit();
	simulator.WriteNOROutputsToFile();

	auto program_end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> setup_duration = simulation_start - program_start;
	std::chrono::duration<double, std::milli> simulation_duration = program_end - simulation_start;
	std::chrono::duration<double, std::milli> program_duration = program_end - program_start;
	char output_char[200];
	sprintf(output_char, "Simulation setup took\t%15.2fms\nSimulation itself took\t%15.2fms\nWhole program took\t%15.2fms\n",
	        setup_duration.count(), simulation_duration.count(), program_duration.count());
	std::cout << std::endl
	          << "Summary of spent time" << std::endl
	          << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl
	          << output_char;
	return 0;
}