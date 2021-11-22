

#include "circuit_simulator.h"

#include <algorithm>
#include <fstream>
#include <iostream>

#include "circuit_file_parser.h"
#include "nor_gate.h"

void CircuitSimulator::InitializeCircuit(const std::string& file_path) {
	parser.ParseFile(file_path);
	bool valid_file = parser.PerformSanityCheck();
	if (!valid_file) {
		return;  // TODO: report back that file is not valid
	}
	InitializeInputs();
	InitializeNORGates();
	DetermineGatesInitialValues();
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

void CircuitSimulator::InitializeNORGates() {
	std::vector<ParsedGate> parsed_nor_gates = parser.GetGates();
	std::sort(parsed_nor_gates.begin(), parsed_nor_gates.end(), &ParsedNORGateSorter);
	for (auto it = parsed_nor_gates.begin(); it != parsed_nor_gates.end(); it++) {
		std::shared_ptr<NORGate> nor_gate(new NORGate(it->gate_name, it->ouput_name, -1.0));
		nor_gates.push_back(nor_gate);
	}

	// add connections to nor gates
	std::shared_ptr<TransitionSource> gnd_potential(new GNDPotential);
	std::shared_ptr<TransitionSource> vdd_potential(new VDDPotential);
	int i = 0;
	for (auto it = parsed_nor_gates.begin(); it != parsed_nor_gates.end(); it++) {
		std::string input_a = it->input_a_name;
		std::string input_b = it->input_b_name;

		int input_a_nor_gate_index = GetNORGateIndexFromOutputName(input_a);
		int input_a_nor_input_index = GetCircuitInputIndexFromOutputName(input_a);
		if (input_a.compare("GND") == 0) {
			nor_gates[i]->SetInputSource(gnd_potential, Input_A);
		} else if (input_a.compare("VDD") == 0) {
			nor_gates[i]->SetInputSource(vdd_potential, Input_A);
		} else if (input_a_nor_gate_index != -1) {
			std::shared_ptr<TransitionSource> nor_gate_output(nor_gates[input_a_nor_gate_index]);
			nor_gates[i]->SetInputSource(nor_gate_output, Input_A);
			NORGateInput subscriber = NORGateInput{nor_gates[i], Input_A};
			nor_gates[input_a_nor_gate_index]->AddSubscriber(subscriber);
		} else if (input_a_nor_input_index != -1) {
			std::shared_ptr<TransitionSource> circuit_input(circuit_inputs[input_a_nor_input_index]);
			nor_gates[i]->SetInputSource(circuit_input, Input_A);
			NORGateInput subscriber = NORGateInput{nor_gates[i], Input_A};
			circuit_inputs[input_a_nor_input_index]->AddSubscriber(subscriber);
		}

		int input_b_nor_gate_index = GetNORGateIndexFromOutputName(input_b);
		int input_b_nor_input_index = GetCircuitInputIndexFromOutputName(input_b);
		if (input_b.compare("GND") == 0) {
			nor_gates[i]->SetInputSource(gnd_potential, Input_B);
		} else if (input_b.compare("VDD") == 0) {
			nor_gates[i]->SetInputSource(vdd_potential, Input_B);
		} else if (input_b_nor_gate_index != -1) {
			std::shared_ptr<TransitionSource> nor_gate_output(nor_gates[input_b_nor_gate_index]);
			nor_gates[i]->SetInputSource(nor_gate_output, Input_B);
			NORGateInput subscriber = NORGateInput{nor_gates[i], Input_B};
			nor_gates[input_b_nor_gate_index]->AddSubscriber(subscriber);
		} else if (input_b_nor_input_index != -1) {
			std::shared_ptr<TransitionSource> circuit_input(circuit_inputs[input_b_nor_input_index]);
			nor_gates[i]->SetInputSource(circuit_input, Input_B);
			NORGateInput subscriber = NORGateInput{nor_gates[i], Input_B};
			circuit_inputs[input_b_nor_input_index]->AddSubscriber(subscriber);
		}

		i++;
	}
}

int CircuitSimulator::GetNORGateIndexFromOutputName(const std::string& name) {
	int left = 0;
	int right = nor_gates.size() - 1;

	while (left <= right) {
		int middle = left + (right - left) / 2;
		if (nor_gates[middle]->GetOutputName().compare(name) == 0) {
			return middle;
		} else {
			if (nor_gates[middle]->GetOutputName().compare(name) > 0) {
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
	std::vector<std::vector<int>> clauses;
	std::vector<int> true_constant = {1};
	clauses.push_back(true_constant);

	for (auto it = nor_gates.begin(); it != nor_gates.end(); it++) {
		InitialValue input_a_initial = (*it)->GetInputSource(Input_A)->GetInitialOutputValue();
		InitialValue input_b_initial = (*it)->GetInputSource(Input_B)->GetInitialOutputValue();

		int input_a_var_index, input_b_var_index;

		if (input_a_initial == UNDEFINED) {
			// input a is neither constant GDD/VDD nor connected to an input
			std::string input_a_node_name = (*it)->GetInputSource(Input_A)->GetOutputName();
			int input_a_nor_gate_index = GetNORGateIndexFromOutputName(input_a_node_name);
			input_a_var_index = 2 + input_a_nor_gate_index;
		} else if (input_a_initial == VDD) {
			input_a_var_index = 1;
		} else {
			input_a_var_index = -1;
		}

		if (input_b_initial == UNDEFINED) {
			// input a is neither constant GDD/VDD nor connected to an input
			std::string input_b_node_name = (*it)->GetInputSource(Input_B)->GetOutputName();
			int input_b_nor_gate_index = GetNORGateIndexFromOutputName(input_b_node_name);
			input_b_var_index = 2 + input_b_nor_gate_index;
		} else if (input_b_initial == VDD) {
			input_b_var_index = 1;
		} else {
			input_b_var_index = -1;
		}

		int output_gate_index = GetNORGateIndexFromOutputName((*it)->GetOutputName());
		int output_var_index = 2 + output_gate_index;

		std::vector<int> first_subclause = {input_a_var_index, input_b_var_index, output_var_index};
		std::vector<int> second_subclause = {-input_a_var_index, -output_var_index};
		std::vector<int> third_subclause = {-input_b_var_index, -output_var_index};

		clauses.push_back(first_subclause);
		clauses.push_back(second_subclause);
		clauses.push_back(third_subclause);
	}

	std::ofstream out_fs("initial_values_determination.dimacs");
	out_fs << "p cnf " << std::to_string(nor_gates.size() + 1) << " " << std::to_string(3 * nor_gates.size() + 1) << "\n";
	for (auto clause = clauses.begin(); clause != clauses.end(); clause++) {
		for (auto literal = clause->begin(); literal != clause->end(); literal++) {
			out_fs << std::to_string(*literal) << " ";
		}
		out_fs << " 0\n";
	}
	out_fs.close();
	// check using minisat and throw away stdout and stderr
	system("minisat initial_values_determination.dimacs initial_values_determination.output > /dev/null 2>&1");
}

bool CircuitSimulator::ParsedNORGateSorter(const ParsedGate& lhs, const ParsedGate& rhs) {
	return lhs.gate_name < rhs.gate_name;
}

bool CircuitSimulator::ParsedInputSorter(const ParsedInput& lhs, const ParsedInput& rhs) {
	return lhs.node_name < rhs.node_name;
}

CircuitSimulator::~CircuitSimulator() {
}