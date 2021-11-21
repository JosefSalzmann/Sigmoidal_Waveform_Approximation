

#include "circuit_simulator.h"

#include <fstream>

#include "circuit_file_parser.h"

void CircuitSimulator::InitializeCircuit(const std::string& file_path) {
	parser = CircuitFileParser();
	parser.ParseFile(file_path);
	bool valid_file = parser.PerformSanityCheck();
	if (!valid_file) {
		return;  // TODO: report back that file is not valid
	}
	InitializeNORGates();
}

void CircuitSimulator::InitializeNORGates() {
	std::vector<ParsedGate> parsed_nor_gates = parser.GetGates();
	for (auto it = parsed_nor_gates.begin(); it != parsed_nor_gates.end(); it++) {
		NORGate nor_gate = NORGate((*it).gate_name, (*it).ouput_name, -1.0);
		nor_gates.push_back(nor_gate);
	}
}

void CircuitSimulator::GetInputInitialValues() {
	std::vector<ParsedInput> parsed_inputs = parser.GetInputs();
	for (auto it = parsed_inputs.begin(); it != parsed_inputs.end(); it++) {
	}
}

InitialValue CircuitSimulator::GetInputFileInitialValue(std::string file_name) {
	std::ifstream input_file_stream(file_name);
	std::string line = "";
	getline(input_file_stream, line);
	if (line.compare("") == 0) {
		return GND;
	} else {
		// TODO: add check if ',' is present
		int steepness_param_end = line.find(',');
		std::string steepness_param_str = line.substr(0, steepness_param_end);
		double steepness_param = atof(steepness_param_str.c_str());

		if (steepness_param < 0) {
			// falling transition => initial values is VDD
			return VDD;
		} else {
			// rising transition => initial values is GND
			return GND;
		}
	}
}
CircuitSimulator::~CircuitSimulator() {
}