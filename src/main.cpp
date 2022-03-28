#include <getopt.h>
#include <plog/Initializers/RollingFileInitializer.h>
#include <plog/Log.h>

#include <chrono>
#include <fstream>
#include <iostream>

#include "cgal_test.h"
#include "circuit_simulator.h"
#include "pcl_test.h"

void Usage(const std::string& prog_name) {
	std::cout << prog_name << " [Options]" << std::endl
	          << "Options:" << std::endl
	          << "-h	Print this message" << std::endl
	          << "-c	Path to circuit file" << std::endl;
}

int main(int argc, char* argv[]) {
	PCLTest pcl_test;
	pcl_test.test();
	// CGALTest cgal_test;
	// cgal_test.test();
	std::string file_name;
	while (true) {
		switch (getopt(argc, argv, "c:h")) {
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