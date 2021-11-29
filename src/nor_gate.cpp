
#include "nor_gate.h"

#include <string>

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

std::shared_ptr<Transition> NORGate::PropagateTransition(const Transition& transition, Input input, const std::shared_ptr<TransitionSchedule>& schedule) {
	return nullptr;
}

/*
 *	Check if transition if the other input also had a transition near to this
 * 	one. If yes, use MIS transfer functions to calculate the output transition.
 */
bool NORGate::CheckIfMIS(const Transition& transition, Input input) {
	double current_tr_shift = transition.parameters.shift;
	if (input == Input_A) {
		if (input_b_transitions.size() == 0)
			return false;
		auto latest_b_tr = input_b_transitions.back();
		double latest_b_tr_shift = latest_b_tr->parameters.shift;
		// TODO make the 1.0 configurable
		if (current_tr_shift - latest_b_tr_shift > 1.0) {
			return true;
		}
	} else {
		if (input_a_transitions.size() == 0)
			return false;
		auto latest_a_tr = input_a_transitions.back();
		double latest_a_tr_shift = latest_a_tr->parameters.shift;
		// TODO make the 1.0 configurable
		if (current_tr_shift - latest_a_tr_shift > 1.0) {
			return true;
		}
	}
	return false;
}

NORGate::~NORGate() {
}