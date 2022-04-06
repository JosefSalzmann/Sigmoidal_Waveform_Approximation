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
	std::shared_ptr<TransitionSchedule> transition_schedule;
	std::shared_ptr<TFCollection> transfer_functions;

	void InitializeNORGates();
	void InitializeInputs();
	void DetermineGatesInitialValues();
	static bool ParsedNORGateSorter(const ParsedGate& lhs, const ParsedGate& rhs);
	static bool ParsedInputSorter(const ParsedInput& lhs, const ParsedInput& rhs);
	int GetNORGateIndexFromOutputName(const std::string& name);
	int GetCircuitInputIndexFromOutputName(const std::string& name);
	void SetNORGateSubscirbersInputValue(std::shared_ptr<NORGate> nor_gate, InitialValue initial_value);
	void InitializeTransferFunctions();

   public:
	CircuitSimulator() : nor_gates{},
	                     circuit_inputs{},
	                     parser{CircuitFileParser()},
	                     transition_schedule{},
	                     transfer_functions{} {};
	~CircuitSimulator();
	void InitializeCircuit(const std::string& file_path);
	std::shared_ptr<TransferFunction> InitializeTransferFunction(ParsedTFModel sis_transfer_function, TFModelType model_type, ANNSISTYPE ty_type);
	void SimulateCircuit();
	void WriteNOROutputsToFile();
};
#endif
