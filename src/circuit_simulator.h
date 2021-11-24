/*
*   Parser for circuit files
*/
#ifndef CIRCUIT_SIMULATOR_H
#define CIRCUIT_SIMULATOR_H

#include "circuit_file_parser.h"
#include "circuit_input.h"
#include "nor_gate.h"
#include "transition_schedule.h"

class CircuitSimulator {
   private:
	std::vector<std::shared_ptr<NORGate>> nor_gates;
	std::vector<std::shared_ptr<CircuitInput>> circuit_inputs;
	CircuitFileParser parser;
	TransitionSchedule transition_schedule;

	void InitializeNORGates();
	void InitializeInputs();
	void DetermineGatesInitialValues();
	static bool ParsedNORGateSorter(const ParsedGate& lhs, const ParsedGate& rhs);
	static bool ParsedInputSorter(const ParsedInput& lhs, const ParsedInput& rhs);
	int GetNORGateIndexFromOutputName(const std::string& name);
	int GetCircuitInputIndexFromOutputName(const std::string& name);
	void SetNORGateSubscirbersInputValue(std::shared_ptr<NORGate> nor_gate, InitialValue initial_value);

   public:
	CircuitSimulator() : nor_gates{},
	                     circuit_inputs{},
	                     parser{CircuitFileParser()},
	                     transition_schedule{TransitionSchedule()} {};
	~CircuitSimulator();
	void InitializeCircuit(const std::string& file_path);
	void SimulateCircuit();
};
#endif
