#include <getopt.h>
#include <plog/Initializers/RollingFileInitializer.h>
#include <plog/Log.h>

#include <chrono>
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

	auto simulation_stop = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> duration_double = simulation_stop - simulation_start;
	std::cerr << std::endl
	          << "Simulation took " << duration_double.count() << "ms" << std::endl;

	return 0;
}