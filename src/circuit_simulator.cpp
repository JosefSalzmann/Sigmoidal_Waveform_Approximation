

#include "circuit_simulator.h"

#include <fstream>

#include "circuit_file_parser.h"

void CircuitSimulator::InitializeCircuit(const std::string& file_path) {
	parser.ParseFile(file_path);
	bool valid_file = parser.PerformSanityCheck();
	if (!valid_file) {
		return;  // TODO: report back that file is not valid
	}
	InitializeNORGates();
	GetInputInitialValues();
}

void CircuitSimulator::InitializeNORGates() {
	std::vector<ParsedGate> parsed_nor_gates = parser.GetGates();
	for (auto it = parsed_nor_gates.begin(); it != parsed_nor_gates.end(); it++) {
		NORGate nor_gate = NORGate((*it).gate_name, (*it).ouput_name, -1.0);
		nor_gates.push_back(nor_gate);
	}
}

/*
 * Get the initial values of all inputs.
 */
void CircuitSimulator::GetInputInitialValues() {
	std::vector<ParsedInput> parsed_inputs = parser.GetInputs();
	for (auto it = parsed_inputs.begin(); it != parsed_inputs.end(); it++) {
		CircuitInput input = CircuitInput((*it).file_name, (*it).node_name);
		input.DetermineInitialValue();
		circuit_inputs.push_back(input);
	}
}

void CircuitSimulator::DetermineGatesInitialValues() {
	bool gate_initialized[nor_gates.size()];
	for (int i = 0; i < (int)nor_gates.size(); i++)
		gate_initialized[i] = false;

	bool all_gates_initialized = false;
	while (!all_gates_initialized) {
		int i = 0;
		for (auto it = nor_gates.begin(); it != nor_gates.end(); it++) {
			if (!gate_initialized[i]) {
			}
			i++;
		}
	}
}

CircuitSimulator::~CircuitSimulator() {
}