
#include "circuit_input.h"

#include <cerrno>
#include <fstream>
#include <iostream>
#include <string>

std::vector<NORGateInput>& CircuitInput::GetSubscribers() {
	return subscribers;
}

std::string CircuitInput::GetOutputName() {
	return node_name;
}

void CircuitInput::AddSubscriber(NORGateInput subscriber) {
	subscribers.push_back(subscriber);
}

std::vector<std::shared_ptr<Transition>>& CircuitInput::GetTransitions() {
	return transitions;
}

/*
 * Get the initial value of an input by looking at the first transition.
 */
void CircuitInput::DetermineInitialValue() {
	std::ifstream input_file_stream(file_name);
	if (input_file_stream.fail()) {
		std::cerr << "File not found: " << file_name << std::endl;
		throw std::exception();
	}
	std::string line = "";
	getline(input_file_stream, line);
	if (line.compare("") == 0) {
		// assume GND as initial value if first line is empty (file will be treated as empty)
		initial_value = GND;
	} else {
		TransitionParameters first_transition = ParseInputFileLine(line);

		if (first_transition.steepness > 0) {
			// rising transition => initial values is GND
			initial_value = GND;
		} else {
			// falling transition => initial values is VDD
			initial_value = VDD;
		}
	}
}

TransitionParameters CircuitInput::ParseInputFileLine(const std::string& line) {
	// check if ',' is present
	auto steepness_param_end = line.find(',');
	if (steepness_param_end == std::string::npos) {
		std::cerr << "invalid line in " << file_name << ": " << line << std::endl;
		throw std::exception();
	}

	std::string steepness_param_str = line.substr(0, steepness_param_end);
	char* endptr;
	errno = 0;
	double steepness_param = std::strtod(steepness_param_str.c_str(), &endptr);
	if (*endptr != '\0' ||  // error, we didn't consume the entire string
	    errno != 0)         // error, overflow or underflow
	{
		std::cerr << "conversion error in " << file_name << ": " << line << std::endl;
		throw std::exception();
	}

	std::string shift_param_str = line.substr(steepness_param_end + 1, line.size() - steepness_param_end - 1);
	double shift_param = std::strtod(shift_param_str.c_str(), &endptr);
	if (             /* *endptr != '\0' ||  // error, we didn't consume the entire string*/
	    errno != 0)  // error, overflow or underflow
	{
		std::cerr << "conversion error in " << file_name << ": " << line << std::endl;
		throw std::exception();
	}

	return {steepness_param, shift_param};
}

void CircuitInput::ReadTransitionsFromInputFile() {
	std::ifstream input_file_stream(file_name);
	if (input_file_stream.fail()) {
		std::cerr << "File not found: " << file_name << std::endl;
		throw std::exception();
	}
	std::string line = "";
	while (getline(input_file_stream, line)) {
		if (line.compare("") == 0) {
			return;
		}
		TransitionParameters parameters = ParseInputFileLine(line);
		std::shared_ptr<Transition> transition(new Transition);
		transition->source = std::shared_ptr<TransitionSource>(shared_from_this());
		transition->sinks = subscribers;
		transition->parameters = parameters;
		transition->parents = {};
		transition->children = {};
		transition->cancelation = false;
		transitions.push_back(transition);
	}
}

InitialValue CircuitInput::GetInitialOutputValue() {
	return initial_value;
}

CircuitInput::~CircuitInput() {
}