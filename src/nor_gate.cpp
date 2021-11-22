
#include "nor_gate.h"

#include <string>

#include "datatypes.h"

void NORGate::SetInputSource(const std::shared_ptr<TransitionSource>& input_source, Input input) {
	if (input == Input_A) {
		input_a_source = input_source;
	} else if (input == Input_B) {
		input_b_source = input_source;
	}
}

std::shared_ptr<TransitionSource>& NORGate::GetInputSource(Input input) {
	if (input == Input_A) {
		return input_a_source;
	} else {
		return input_b_source;
	}
}

void NORGate::AddSubscriber(NORGateInput subscriber) {
	subscribers.push_back(subscriber);
}

void NORGate::SetInitialInput(InitialValue initial_value, Input input) {
	// set initial input value
	if (input == Input_A) {
		initial_value_input_a = initial_value;
	} else if (input == Input_B) {
		initial_value_input_b = initial_value;
	}

	// compute new initial output value
	if (initial_value_input_a == UNDEFINED || initial_value_input_b == UNDEFINED) {
		initial_value_output = UNDEFINED;
	} else if (!(initial_value_input_a == VDD || initial_value_input_b == VDD)) {
		initial_value_output = VDD;
	} else {
		initial_value_output = GND;
	}
}

std::vector<NORGateInput>& NORGate::GetSubscribers() {
	return subscribers;
}

std::string NORGate::GetOutputName() {
	return output_node_name;
}

bool NORGate::NORGateSorter(const NORGate& lhs, const NORGate& rhs) {
	return lhs.gate_name < rhs.gate_name;
}

InitialValue NORGate::GetInitialOutputValue() {
	return initial_value_output;
}
// bool NORGate::operator<(const NORGate& gate) const {
// 	return gate_name < gate.gate_name;
// }

// bool NORGate::operator<(const std::string& string) const {
// 	return gate_name < string;
// }

// bool NORGate::operator<(const char char_ar[]) const {
// 	std::string str(char_ar);
// 	return gate_name < str;
// }

NORGate::~NORGate() {
}