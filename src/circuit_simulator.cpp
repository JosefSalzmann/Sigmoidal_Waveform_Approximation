/*
  File: circuit_simulator.cpp
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

#include "circuit_simulator.h"

#include <assert.h>
#include <cryptominisat5/cryptominisat.h>

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

#include "ann_sis_transfer_function.h"
#include "circuit_file_parser.h"
#include "logic_gate.h"
#include "polynomial_sis_transfer_function.h"

void CircuitSimulator::InitializeCircuit(const std::string& file_path) {
	parser.ParseFile(file_path);
	bool valid_file = parser.PerformSanityCheck();
	if (!valid_file) {
		return;  // TODO: report back that file is not valid
	}

	InitializeTransferFunctions();
	InitializeInputs();
	InitializeNORGates();
	DetermineGatesInitialValues();

	// read in all input transitions
	transition_schedule = std::make_shared<TransitionSchedule>();
	for (auto input = circuit_inputs.begin(); input != circuit_inputs.end(); input++) {
		(*input)->ReadTransitionsFromInputFile();
		auto input_transitions = (*input)->GetTransitions();
		for (auto in_transition = input_transitions.begin(); in_transition != input_transitions.end(); in_transition++) {
			transition_schedule->AddFutureTransition((*in_transition));
		}
	}
	transition_schedule->SortFutureTransitions();
}

/*
 * Get the initial values of all inputs.
 */
void CircuitSimulator::InitializeInputs() {
	std::vector<ParsedInput> parsed_inputs = parser.GetInputs();
	std::sort(parsed_inputs.begin(), parsed_inputs.end(), &ParsedInputSorter);
	for (auto it = parsed_inputs.begin(); it != parsed_inputs.end(); it++) {
		std::shared_ptr<CircuitInput> input(new CircuitInput(it->file_name, it->node_name));
		input->DetermineInitialValue();
		circuit_inputs.push_back(input);
	}
}

/*
 * Set the  input sources and the subscribers for all NOR Gates and the
 * subscribers of the inputs.
 */
void CircuitSimulator::InitializeNORGates() {
	std::vector<ParsedGate> parsed_gates = parser.GetGates();
	std::sort(parsed_gates.begin(), parsed_gates.end(), &ParsedNORGateSorter);
	for (auto it = parsed_gates.begin(); it != parsed_gates.end(); it++) {
		FunctionType gate_type;
		if (it->gate_type.compare("NOR") == 0) {
			gate_type = NOR;
		} else if (it->gate_type.compare("INV") == 0) {
			gate_type = INV;
		} else {
			std::cout << "Unknown gate type:" << it->gate_type;
			exit(-1);
		}
		// TODO: make default steepnesses configurable
		std::shared_ptr<LogicGate> logic_gate(new LogicGate(gate_type, it->gate_name, it->ouput_name, transfer_functions, -100, 100, logging));
		logic_gates.push_back(logic_gate);
	}

	// add connections to gates
	std::shared_ptr<TransitionSource> gnd_potential(new GNDPotential);
	std::shared_ptr<TransitionSource> vdd_potential(new VDDPotential);
	int i = 0;
	for (auto it = parsed_gates.begin(); it != parsed_gates.end(); it++) {
		std::string input_a = it->input_a_name;
		std::string input_b = it->input_b_name;

		int input_a_gate_index = GetGateIndexFromOutputName(input_a);
		int input_a_input_index = GetCircuitInputIndexFromOutputName(input_a);
		if (input_a.compare("GND") == 0) {
			logic_gates[i]->SetInputSource(gnd_potential, Input_A);
		} else if (input_a.compare("VDD") == 0) {
			logic_gates[i]->SetInputSource(vdd_potential, Input_A);
		} else if (input_a_gate_index != -1) {
			std::shared_ptr<TransitionSource> nor_gate_output(logic_gates[input_a_gate_index]);
			logic_gates[i]->SetInputSource(nor_gate_output, Input_A);
			NORGateInput subscriber = NORGateInput{logic_gates[i], Input_A};
			logic_gates[input_a_gate_index]->AddSubscriber(subscriber);
		} else if (input_a_input_index != -1) {
			std::shared_ptr<TransitionSource> circuit_input(circuit_inputs[input_a_input_index]);
			logic_gates[i]->SetInputSource(circuit_input, Input_A);
			NORGateInput subscriber = NORGateInput{logic_gates[i], Input_A};
			circuit_inputs[input_a_input_index]->AddSubscriber(subscriber);
		}

		if (it->gate_type.compare("INV") == 0) {  // inverter does not have an input b
			i++;
			continue;
		}
		int input_b_nor_gate_index = GetGateIndexFromOutputName(input_b);
		int input_b_nor_input_index = GetCircuitInputIndexFromOutputName(input_b);
		if (input_b.compare("GND") == 0) {
			logic_gates[i]->SetInputSource(gnd_potential, Input_B);
		} else if (input_b.compare("VDD") == 0) {
			logic_gates[i]->SetInputSource(vdd_potential, Input_B);
		} else if (input_b_nor_gate_index != -1) {
			std::shared_ptr<TransitionSource> nor_gate_output(logic_gates[input_b_nor_gate_index]);
			logic_gates[i]->SetInputSource(nor_gate_output, Input_B);
			NORGateInput subscriber = NORGateInput{logic_gates[i], Input_B};
			logic_gates[input_b_nor_gate_index]->AddSubscriber(subscriber);
		} else if (input_b_nor_input_index != -1) {
			std::shared_ptr<TransitionSource> circuit_input(circuit_inputs[input_b_nor_input_index]);
			logic_gates[i]->SetInputSource(circuit_input, Input_B);
			NORGateInput subscriber = NORGateInput{logic_gates[i], Input_B};
			circuit_inputs[input_b_nor_input_index]->AddSubscriber(subscriber);
		}

		i++;
	}
}

int CircuitSimulator::GetGateIndexFromOutputName(const std::string& name) {
	int left = 0;
	int right = logic_gates.size() - 1;

	while (left <= right) {
		int middle = left + (right - left) / 2;
		if (logic_gates[middle]->GetOutputName().compare(name) == 0) {
			return middle;
		} else {
			if (logic_gates[middle]->GetOutputName().compare(name) > 0) {
				right = middle - 1;
			} else {
				left = middle + 1;
			}
		}
	}
	return -1;
}

int CircuitSimulator::GetCircuitInputIndexFromOutputName(const std::string& name) {
	int left = 0;
	int right = circuit_inputs.size() - 1;

	while (left <= right) {
		int middle = left + (right - left) / 2;
		if (circuit_inputs[middle]->GetOutputName().compare(name) == 0) {
			return middle;
		} else {
			if (circuit_inputs[middle]->GetOutputName().compare(name) > 0) {
				right = middle - 1;
			} else {
				left = middle + 1;
			}
		}
	}
	return -1;
}

/*
 * Determine initial values of all circuit nodes, using a SAT solver
 * with n nor gates the variables 2,3,4,...,n+1 correspond to the nor gates intial output states
 * variable 1 serves as constant for true and false
 * where: var 2 = output state of first gate
 * 		  var 3 = output state of second gate
 * 			...	=	...
 */
void CircuitSimulator::DetermineGatesInitialValues() {
	CMSat::SATSolver solver;
	solver.set_num_threads(1);
	solver.new_vars(1 + logic_gates.size());
	std::vector<CMSat::Lit> clause;

	// add a constant for true and false
	clause.push_back(CMSat::Lit(0, false));
	solver.add_clause(clause);

	for (auto it = logic_gates.begin(); it != logic_gates.end(); it++) {
		if ((*it)->GetType() == NOR) {
			InitialValue input_a_initial = (*it)->GetInputSource(Input_A)->GetInitialOutputValue();
			InitialValue input_b_initial = (*it)->GetInputSource(Input_B)->GetInitialOutputValue();

			CMSat::Lit input_a_lit, input_a_lit_neg, input_b_lit, input_b_lit_neg;
			if (input_a_initial == UNDEFINED) {
				// input a is neither constant GDD/VDD nor connected to an input
				std::string input_a_node_name = (*it)->GetInputSource(Input_A)->GetOutputName();
				int input_a_gate_index = GetGateIndexFromOutputName(input_a_node_name);
				input_a_lit = CMSat::Lit(1 + input_a_gate_index, false);
				input_a_lit_neg = CMSat::Lit(1 + input_a_gate_index, true);
			} else if (input_a_initial == VDD) {
				input_a_lit = CMSat::Lit(0, false);
				input_a_lit_neg = CMSat::Lit(0, true);
				(*it)->SetInitialInput(VDD, Input_A);
			} else {
				input_a_lit = CMSat::Lit(0, true);
				input_a_lit_neg = CMSat::Lit(0, false);
				(*it)->SetInitialInput(GND, Input_A);
			}

			if (input_b_initial == UNDEFINED) {
				// input a is neither constant GDD/VDD nor connected to an input
				std::string input_b_node_name = (*it)->GetInputSource(Input_B)->GetOutputName();
				int input_b_nor_gate_index = GetGateIndexFromOutputName(input_b_node_name);
				input_b_lit = CMSat::Lit(1 + input_b_nor_gate_index, false);
				input_b_lit_neg = CMSat::Lit(1 + input_b_nor_gate_index, true);
			} else if (input_b_initial == VDD) {
				input_b_lit = CMSat::Lit(0, false);
				input_b_lit_neg = CMSat::Lit(0, true);
				(*it)->SetInitialInput(VDD, Input_B);
			} else {
				input_b_lit = CMSat::Lit(0, true);
				input_b_lit_neg = CMSat::Lit(0, false);
				(*it)->SetInitialInput(GND, Input_B);
			}

			int output_gate_index = GetGateIndexFromOutputName((*it)->GetOutputName());
			CMSat::Lit output_lit = CMSat::Lit(1 + output_gate_index, false);
			CMSat::Lit output_lit_neg = CMSat::Lit(1 + output_gate_index, true);

			/* for every NOR gate add the input output relation to the clause set:
			 *  		(InputA NOR InputB) XNOR Output
			 * which converted to cnf is equal to:
			 * 			(InputA OR InputB OR Output) AND
			 *			(!InputA OR !Output) AND
			 *			(!InputB OR !Output) AND
			 */
			std::vector<CMSat::Lit> first_clause;
			first_clause.push_back(input_a_lit);
			first_clause.push_back(input_b_lit);
			first_clause.push_back(output_lit);

			std::vector<CMSat::Lit> second_clause;
			second_clause.push_back(input_a_lit_neg);
			second_clause.push_back(output_lit_neg);

			std::vector<CMSat::Lit> third_clause;
			third_clause.push_back(input_b_lit_neg);
			third_clause.push_back(output_lit_neg);

			solver.add_clause(first_clause);
			solver.add_clause(second_clause);
			solver.add_clause(third_clause);
		} else {
			InitialValue input_a_initial = (*it)->GetInputSource(Input_A)->GetInitialOutputValue();

			CMSat::Lit input_a_lit, input_a_lit_neg;
			if (input_a_initial == UNDEFINED) {
				// input a is neither constant GDD/VDD nor connected to an input
				std::string input_a_node_name = (*it)->GetInputSource(Input_A)->GetOutputName();
				int input_a_gate_index = GetGateIndexFromOutputName(input_a_node_name);
				input_a_lit = CMSat::Lit(1 + input_a_gate_index, false);
				input_a_lit_neg = CMSat::Lit(1 + input_a_gate_index, true);
			} else if (input_a_initial == VDD) {
				input_a_lit = CMSat::Lit(0, false);
				input_a_lit_neg = CMSat::Lit(0, true);
				(*it)->SetInitialInput(VDD, Input_A);
			} else {
				input_a_lit = CMSat::Lit(0, true);
				input_a_lit_neg = CMSat::Lit(0, false);
				(*it)->SetInitialInput(GND, Input_A);
			}

			int output_gate_index = GetGateIndexFromOutputName((*it)->GetOutputName());
			CMSat::Lit output_lit = CMSat::Lit(1 + output_gate_index, false);
			CMSat::Lit output_lit_neg = CMSat::Lit(1 + output_gate_index, true);

			/* for every Inverter add the input output relation to the clause set:
			 *  		(not InputA) XNOR Output
			 * which converted to cnf is equal to:
			 *			( InputA OR  Output) AND
			 *			(!InputA OR !Output) AND
			 */

			std::vector<CMSat::Lit> first_clause;
			first_clause.push_back(input_a_lit);
			first_clause.push_back(output_lit);

			std::vector<CMSat::Lit> second_clause;
			second_clause.push_back(input_a_lit_neg);
			second_clause.push_back(output_lit_neg);

			solver.add_clause(first_clause);
			solver.add_clause(second_clause);
		}
	}

	bool solvable = solver.solve() == CMSat::l_True;
	if (!solvable) {
		std::cerr << "Circuit initial values cannot be computed" << std::endl;
		throw std::exception();
	}

	// recover the initial output values from the sat-solver model and apply them
	int model_index = 1;
	for (auto it = logic_gates.begin(); it != logic_gates.end(); it++) {
		bool output_value = solver.get_model()[model_index] == CMSat::l_True;
		InitialValue output_init_value;
		if (output_value) {
			output_init_value = VDD;
		} else {
			output_init_value = GND;
		}

		SetNORGateSubscirbersInputValue((*it), output_init_value);
		model_index++;
	}
}

/*
 * Set the initial input values of the gates subscribers,
 * by setting the initial values of the inputs the initial output value is derived automatically.
 */
void CircuitSimulator::SetNORGateSubscirbersInputValue(std::shared_ptr<LogicGate> nor_gate, InitialValue initial_value) {
	auto subscribers = nor_gate->GetSubscribers();
	for (auto subscriber = subscribers.begin(); subscriber != subscribers.end(); subscriber++) {
		subscriber->nor_gate->SetInitialInput(initial_value, subscriber->input);
	}
}

void CircuitSimulator::InitializeTransferFunctions() {
	std::vector<ParsedTFModel> parsed_tf_models = parser.GetTFModels();

	if (parsed_tf_models.size() != 12) {
		std::cerr << "Transfer functions could not be parsed" << std::endl;
		throw std::exception();
	}

	transfer_functions = std::make_shared<TFCollection>();

	transfer_functions->sis_input_a_falling = InitializeTransferFunction(parsed_tf_models[0], SIS, AFALLING);
	transfer_functions->sis_input_a_rising = InitializeTransferFunction(parsed_tf_models[2], SIS, ARISING);
	transfer_functions->sis_input_b_falling = InitializeTransferFunction(parsed_tf_models[4], SIS, BFALLING);
	transfer_functions->sis_input_b_rising = InitializeTransferFunction(parsed_tf_models[6], SIS, BRISING);
	transfer_functions->inverter_falling = InitializeTransferFunction(parsed_tf_models[8], SIS, INVERTERFALLING);
	transfer_functions->inverter_rising = InitializeTransferFunction(parsed_tf_models[10], SIS, INVERTERRISING);

	transfer_functions->sis_input_a_falling->SetName("sis_input_a_falling");
	transfer_functions->sis_input_a_rising->SetName("sis_input_a_rising");
	transfer_functions->sis_input_b_falling->SetName("sis_input_b_falling");
	transfer_functions->sis_input_b_rising->SetName("sis_input_b_rising");
	transfer_functions->inverter_falling->SetName("inverter_falling");
	transfer_functions->inverter_rising->SetName("inverter_rising");

	transfer_functions->sis_input_a_falling->ReadValidRegionFile(parsed_tf_models[1].file_name);
	transfer_functions->sis_input_a_rising->ReadValidRegionFile(parsed_tf_models[3].file_name);
	transfer_functions->sis_input_b_falling->ReadValidRegionFile(parsed_tf_models[5].file_name);
	transfer_functions->sis_input_b_rising->ReadValidRegionFile(parsed_tf_models[7].file_name);
	transfer_functions->inverter_falling->ReadValidRegionFile(parsed_tf_models[9].file_name);
	transfer_functions->inverter_rising->ReadValidRegionFile(parsed_tf_models[11].file_name);

	// TODO: make this configurable
	double max_shift = 0.15;
	TransitionParameters default_prev_out_rising = {100, max_shift};
	TransitionParameters default_prev_out_falling = {-100, max_shift};

	transfer_functions->sis_input_a_falling->SetDefaultValues(default_prev_out_rising, 0.2);
	transfer_functions->sis_input_a_rising->SetDefaultValues(default_prev_out_falling, 0.2);
	transfer_functions->sis_input_b_falling->SetDefaultValues(default_prev_out_rising, 0.2);
	transfer_functions->sis_input_b_rising->SetDefaultValues(default_prev_out_falling, 0.2);
	transfer_functions->inverter_falling->SetDefaultValues(default_prev_out_rising, max_shift);
	transfer_functions->inverter_rising->SetDefaultValues(default_prev_out_falling, max_shift);
}

std::shared_ptr<TransferFunction> CircuitSimulator::InitializeTransferFunction(ParsedTFModel sis_transfer_function, TFModelType model_type, ANNSISTYPE ty_type) {
	std::string tf_approach = sis_transfer_function.tf_approach;
	std::transform(tf_approach.begin(), tf_approach.end(), tf_approach.begin(), ::toupper);
	std::shared_ptr<TransferFunction> transfer_function;
	if (tf_approach.compare("POLY") == 0) {
		if (model_type == SIS) {
			transfer_function = std::make_shared<PolynomialSISTransferFunction>();
		}
		transfer_function->ReadModel(sis_transfer_function.file_name);

	} else if (tf_approach.compare("ANN") == 0) {
		if (model_type == SIS) {
			transfer_function = std::make_shared<ANNSISTransferFunction>(ty_type, logging);
		} else {
			std::cerr << "ANN MSI not yet supported." << std::endl;
			throw std::exception();
		}
		transfer_function->ReadModel(sis_transfer_function.file_name);
	} else {
		std::cerr << "Unknown transfer functions approach: " << sis_transfer_function.tf_approach << std::endl;
		throw std::exception();
	}

	return transfer_function;
}

void CircuitSimulator::SimulateCircuit() {
	while (transition_schedule->HasFutureTransitions()) {
		auto current_transition = transition_schedule->ConsumeFirstTransition();
		if (current_transition->sinks.size() == 0)
			continue;
		for (auto sink = current_transition->sinks.begin(); sink != current_transition->sinks.end(); sink++) {
			sink->nor_gate->PropagateTransition(current_transition, sink->input, transition_schedule);
		}
		transition_schedule->AddPastTransition(current_transition);
	}
}

void CircuitSimulator::WriteNOROutputsToFile() {
	std::ofstream output_file;
	output_file.open("output.txt");
	// TODO: sanity check output names
	auto parsed_outputs = parser.GetOutputs();
	for (auto it = parsed_outputs.begin(); it != parsed_outputs.end(); it++) {
		std::string output_name = it->node_name;
		for (auto nor_gate = logic_gates.begin(); nor_gate != logic_gates.end(); nor_gate++) {
			if ((*nor_gate)->GetOutputName().compare(output_name) == 0) {
				output_file << output_name << std::endl;
				auto output_transitions = (*nor_gate)->GetOutputTransitions();
				std::stringstream ss;
				for (auto tr = output_transitions.begin() + 1; tr != output_transitions.end(); tr++) {
					if (!(*tr)->cancelation) {
						ss << std::fixed << std::setprecision(5) << (*tr)->parameters.steepness << ",";
						ss << std::fixed << std::setprecision(5) << (*tr)->parameters.shift << std::endl;
					}
				}
				output_file << ss.str() << std::endl;
			}
		}
	}
	output_file.close();
}

bool CircuitSimulator::ParsedNORGateSorter(const ParsedGate& lhs, const ParsedGate& rhs) {
	return lhs.gate_name < rhs.gate_name;
}

bool CircuitSimulator::ParsedInputSorter(const ParsedInput& lhs, const ParsedInput& rhs) {
	return lhs.node_name < rhs.node_name;
}

CircuitSimulator::~CircuitSimulator() {
}