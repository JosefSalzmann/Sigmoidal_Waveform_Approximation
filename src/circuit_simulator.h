/*
*   Parser for circuit files
*/
#ifndef CIRCUIT_SIMULATOR_H
#define CIRCUIT_SIMULATOR_H

#include <string>

#include "circuit_file_parser.h"
#include "nor_gate.h"

struct InputInitialValue {
	std::string input_name;
	InitialValue initial_value;
};

class CircuitSimulator {
   private:
	std::vector<NORGate> nor_gates;
	InputInitialValue input_initial_values;
	CircuitFileParser parser;

	void InitializeNORGates();
	void GetInputInitialValues();
	InitialValue GetInputFileInitialValue(std::string file_name);

   public:
	CircuitSimulator() : nor_gates{} {};
	~CircuitSimulator();
	void InitializeCircuit(const std::string& file_path);
};
#endif
