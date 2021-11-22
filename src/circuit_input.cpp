
#include "circuit_input.h"

#include <fstream>

std::vector<NORGateInput>& CircuitInput::GetSubscribers() {
	return subscribers;
}

std::string CircuitInput::GetOutputName() {
	return node_name;
}

void CircuitInput::AddSubscriber(NORGateInput subscriber) {
	subscribers.push_back(subscriber);
}

/*
 * Get the initial value of an input by looking at the first transition.
 */
void CircuitInput::DetermineInitialValue() {
	std::ifstream input_file_stream(file_name);
	std::string line = "";
	getline(input_file_stream, line);
	if (line.compare("") == 0) {
		// assume GND as initial value if first line is empty (file will be treated as empty)
		initial_value = GND;
	} else {
		// TODO: add check if ',' is present and string input other checks :P !
		int steepness_param_end = line.find(',');
		std::string steepness_param_str = line.substr(0, steepness_param_end);
		double steepness_param = atof(steepness_param_str.c_str());

		if (steepness_param < 0) {
			// falling transition => initial values is VDD
			initial_value = VDD;
		} else {
			// rising transition => initial values is GND
			initial_value = GND;
		}
	}
}

InitialValue CircuitInput::GetInitialOutputValue() {
	return initial_value;
}

CircuitInput::~CircuitInput() {
}