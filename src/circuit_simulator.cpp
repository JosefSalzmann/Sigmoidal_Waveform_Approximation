
#include "circuit_simulator.h"

#include <assert.h>
#include <cryptominisat5/cryptominisat.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

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

	// read in all input transitions
	for (auto input = circuit_inputs.begin(); input != circuit_inputs.end(); input++) {
		(*input)->ReadTransitionsFromInputFile();
	}
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
	CMSat::SATSolver solver;
	solver.set_num_threads(1);
	solver.new_vars(1 + nor_gates.size());
	std::vector<CMSat::Lit> clause;

	// add a constant for true and false
	clause.push_back(CMSat::Lit(0, false));
	solver.add_clause(clause);

	for (auto it = nor_gates.begin(); it != nor_gates.end(); it++) {
		InitialValue input_a_initial = (*it)->GetInputSource(Input_A)->GetInitialOutputValue();
		InitialValue input_b_initial = (*it)->GetInputSource(Input_B)->GetInitialOutputValue();

		CMSat::Lit input_a_lit, input_a_lit_neg, input_b_lit, input_b_lit_neg;
		if (input_a_initial == UNDEFINED) {
			// input a is neither constant GDD/VDD nor connected to an input
			std::string input_a_node_name = (*it)->GetInputSource(Input_A)->GetOutputName();
			int input_a_nor_gate_index = GetNORGateIndexFromOutputName(input_a_node_name);
			input_a_lit = CMSat::Lit(1 + input_a_nor_gate_index, false);
			input_a_lit_neg = CMSat::Lit(1 + input_a_nor_gate_index, true);
		} else if (input_a_initial == VDD) {
			input_a_lit = CMSat::Lit(0, true);
			input_a_lit_neg = CMSat::Lit(0, false);
			(*it)->SetInitialInput(VDD, Input_A);
		} else {
			input_a_lit = CMSat::Lit(0, false);
			input_a_lit_neg = CMSat::Lit(0, true);
			(*it)->SetInitialInput(GND, Input_A);
		}

		if (input_b_initial == UNDEFINED) {
			// input a is neither constant GDD/VDD nor connected to an input
			std::string input_b_node_name = (*it)->GetInputSource(Input_B)->GetOutputName();
			int input_b_nor_gate_index = GetNORGateIndexFromOutputName(input_b_node_name);
			input_b_lit = CMSat::Lit(1 + input_b_nor_gate_index, false);
			input_b_lit_neg = CMSat::Lit(1 + input_b_nor_gate_index, true);
		} else if (input_b_initial == VDD) {
			input_b_lit = CMSat::Lit(0, true);
			input_b_lit_neg = CMSat::Lit(0, false);
			(*it)->SetInitialInput(VDD, Input_B);
		} else {
			input_b_lit = CMSat::Lit(0, false);
			input_b_lit_neg = CMSat::Lit(0, true);
			(*it)->SetInitialInput(GND, Input_B);
		}

		int output_gate_index = GetNORGateIndexFromOutputName((*it)->GetOutputName());
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
	}

	bool solvable = solver.solve() == CMSat::l_True;
	if (!solvable) {
		// TODO: report that no initial assignment can be found
		std::cerr << "Circuit initial values cannot be computed" << std::endl;
		return;
	}

	// recover the initial output values from the sat-solver model and apply them
	int model_index = 1;
	for (auto it = nor_gates.begin(); it != nor_gates.end(); it++) {
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
void CircuitSimulator::SetNORGateSubscirbersInputValue(std::shared_ptr<NORGate> nor_gate, InitialValue initial_value) {
	auto subscribers = nor_gate->GetSubscribers();
	for (auto subscriber = subscribers.begin(); subscriber != subscribers.end(); subscriber++) {
		subscriber->nor_gate->SetInitialInput(initial_value, subscriber->input);
	}
}

bool CircuitSimulator::ParsedNORGateSorter(const ParsedGate& lhs, const ParsedGate& rhs) {
	return lhs.gate_name < rhs.gate_name;
}

bool CircuitSimulator::ParsedInputSorter(const ParsedInput& lhs, const ParsedInput& rhs) {
	return lhs.node_name < rhs.node_name;
}

CircuitSimulator::~CircuitSimulator() {
}