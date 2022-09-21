/*
  File: circuit_simulator.h
  Author: Josef Salzmann <josef.salzmann@tuwien.ac.at>
  Copyright (c) 2022

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CIRCUIT_SIMULATOR_H
#define CIRCUIT_SIMULATOR_H

#include "circuit_file_parser.h"
#include "circuit_input.h"
#include "logic_gate.h"
#include "transition_schedule.h"

class CircuitSimulator {
   private:
	std::vector<std::shared_ptr<LogicGate>> logic_gates;
	std::vector<std::shared_ptr<CircuitInput>> circuit_inputs;
	CircuitFileParser parser;
	std::shared_ptr<TransitionSchedule> transition_schedule;
	std::shared_ptr<TFCollection> transfer_functions;
	bool logging;

	void InitializeNORGates();
	void InitializeInputs();
	void DetermineGatesInitialValues();
	static bool ParsedNORGateSorter(const ParsedGate& lhs, const ParsedGate& rhs);
	static bool ParsedInputSorter(const ParsedInput& lhs, const ParsedInput& rhs);
	int GetGateIndexFromOutputName(const std::string& name);
	int GetCircuitInputIndexFromOutputName(const std::string& name);
	void SetNORGateSubscirbersInputValue(std::shared_ptr<LogicGate> nor_gate, InitialValue initial_value);
	void InitializeTransferFunctions();

   public:
	CircuitSimulator(bool logging) : logic_gates{},
	                                 circuit_inputs{},
	                                 parser{CircuitFileParser()},
	                                 transition_schedule{},
	                                 transfer_functions{},
	                                 logging{logging} {};
	~CircuitSimulator();
	void InitializeCircuit(const std::string& file_path);
	std::shared_ptr<TransferFunction> InitializeTransferFunction(ParsedTFModel sis_transfer_function, TFModelType model_type, ANNSISTYPE ty_type);
	void SimulateCircuit();
	void WriteNOROutputsToFile();
};
#endif
