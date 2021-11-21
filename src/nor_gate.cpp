
#include "nor_gate.h"

#include "datatypes.h"

std::string NORGate::GetName() {
	return gate_name;
}

InitialValue NORGate::GetInitialOutput() {
	return initial_output;
}

InitialValue NORGate::GetInitialInput(Input input) {
	if (input == Input_A) {
		return initial_input_a;
	} else {
		return initial_input_b;
	}
}

void NORGate::SetInitialInput(InitialValue initial_value, Input input) {
	// set initial input value
	if (input == Input_A) {
		initial_input_a = initial_value;
	} else if (input == Input_B) {
		initial_input_b = initial_value;
	}

	// compute new initial output value
	if (initial_input_a == UNDEFINED || initial_input_b == UNDEFINED ||
	    !(initial_input_a == VDD || initial_input_b == VDD)) {
		initial_output = VDD;
	} else {
		initial_output = GND;
	}
}

std::vector<std::shared_ptr<NORGateInput>>& NORGate::GetSubscribers() {
	return subscribers;
}

std::string NORGate::GetOutputNodeName() {
	return output_node_name;
}

NORGate::~NORGate() {
}