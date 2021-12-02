#include <getopt.h>

#include <fstream>
#include <iostream>

#include "circuit_simulator.h"

void Usage(std::string prog_name) {
	std::cout << prog_name << " [Options]" << std::endl
	          << "Options:" << std::endl
	          << "-h	Print this message" << std::endl
	          << "-c	Path to circuit file" << std::endl;
}

int main(int argc, char* argv[]) {
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

	CircuitSimulator simulator = CircuitSimulator();
	try {
		simulator.InitializeCircuit(file_name);
	} catch (const std::exception& e) {
		return -1;
	}
	simulator.SimulateCircuit();
	simulator.WriteNOROutputsToFile();

	return 0;
}