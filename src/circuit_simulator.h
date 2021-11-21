/*
*   Parser for circuit files
*/
#ifndef CIRCUIT_SIMULATOR_H
#define CIRCUIT_SIMULATOR_H

#include <string>

#include "circuit_file_parser.h"
#include "circuit_input.h"
#include "nor_gate.h"

class CircuitSimulator {
   private:
	std::vector<NORGate> nor_gates;
	std::vector<CircuitInput> circuit_inputs;
	CircuitFileParser parser;

	void InitializeNORGates();
	void GetInputInitialValues();
	InitialValue GetInputFileInitialValue(const std::string& file_name);
	void DetermineGatesInitialValues();

   public:
	CircuitSimulator() : nor_gates{}, circuit_inputs{}, parser{CircuitFileParser()} {};
	~CircuitSimulator();
	void InitializeCircuit(const std::string& file_path);
};
#endif
