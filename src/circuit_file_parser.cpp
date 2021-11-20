/*
*   Parser for circuit files
*/

#include "circuit_file_parser.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <set>
#include <string>

#include "datatypes.h"

int CircuitFileParser::ParseFile(const std::string& file_name) {
	int current_section = -1;  // 0 = input, 1 = gates, 2 = outputs

	bool parsing_failed = false;

	std::ifstream input_file_stream(file_name);
	std::string line;
	while (getline(input_file_stream, line)) {
		int index_first_car = 0;
		int line_len = line.length();
		// find index of first non space character
		while (index_first_car < (line_len - 1) && (line[index_first_car] == ' ' || line[index_first_car] == '\t')) {
			index_first_car++;
		}
		// line is comment if first non space character is '*',
		// also skip line if it is empty
		if (line[index_first_car] != '*' && index_first_car != line_len) {
			std::string line_no_front_space = line.substr(index_first_car, line.length() - index_first_car);

			if (line_no_front_space.compare(0, 6, "INPUTS") == 0) {
				current_section = 0;
			} else if (line_no_front_space.compare(0, 5, "GATES") == 0) {
				current_section = 1;
			} else if (line_no_front_space.compare(0, 7, "OUTPUTS") == 0) {
				current_section = 2;
			} else {
				if (current_section == 0) {
					if (LineIsParseableInput(line_no_front_space)) {
						inputs.push_back(GetParsedInput(line));
					} else {
						std::cout << "Cannot parse: " << line << std::endl;
						parsing_failed = true;
						break;
					}
				} else if (current_section == 1) {
					if (LineIsParseableGate(line_no_front_space)) {
						gates.push_back(GetParsedGate(line));
					} else {
						std::cout << "Cannot parse: " << line << std::endl;
						parsing_failed = true;
						break;
					}
				} else if (current_section == 2) {
					if (LineIsParseableOutput(line_no_front_space)) {
						outputs.push_back(GetParsedOutput(line));
					} else {
						std::cout << "Cannot parse: " << line << std::endl;
						parsing_failed = true;
						break;
					}
				}
			}
		}
	}
	input_file_stream.close();
	if (parsing_failed) {
		return -1;
	}
	return 0;
}

/**
 * Check that each circuit node has exactly one source and atleast one sink.
 * Nodes with more than one source cause abortion.
 * TODO: Maybe cause abortion when a sink has no source?
*/
bool CircuitFileParser::PerformSanityCheck() {
	std::multiset<std::string> source_names, sink_names;

	for (auto it = inputs.begin(); it != inputs.end(); it++) {
		source_names.insert((*it).node_name);
	}

	for (auto it = gates.begin(); it != gates.end(); it++) {
		// std::cout << (*it).ouput_name << std::endl;
		source_names.insert((*it).ouput_name);
		sink_names.insert((*it).input1_name);
		sink_names.insert((*it).input2_name);
	}

	for (auto it = outputs.begin(); it != outputs.end(); it++) {
		source_names.insert((*it).node_name);
	}

	// look for duplicates in the sink multi set
	auto duplicate = std::adjacent_find(source_names.begin(), source_names.end());
	if (duplicate != source_names.end()) {
		std::cout << "Error, circuit node " << *duplicate << " has multiple sources." << std::endl;
		return false;
	}

	// look sinks that are not connected to a source
	// for (auto it = sink_names.begin(); it != sink_names.end(); it++) {
	// 	auto source = source_names.find(*it);
	// 	if (source == source_names.end()) {
	// 		std::cout << "Error, circuit node " << *it << " is not connected." << std::endl;
	// 	}
	// 	return false;
	// }
	return true;
}

std::vector<ParsedInput>& CircuitFileParser::GetInputs() {
	return inputs;
}
std::vector<ParsedGate>& CircuitFileParser::GetGates() {
	return gates;
}
std::vector<ParsedOutput>& CircuitFileParser::GetOutputs() {
	return outputs;
}

int CircuitFileParser::CountSpaces(const std::string& line) {
	int num_spaces = 0;
	for (int i = 0; i < (int)line.length(); i++) {
		if (line[i] == ' ')
			num_spaces++;
	}
	return num_spaces;
}
bool CircuitFileParser::LineIsParseableInput(const std::string& line) {
	if (CountSpaces(line) == 1)
		return true;
	else
		return false;
}
bool CircuitFileParser::LineIsParseableGate(const std::string& line) {
	if (CountSpaces(line) == 3)
		return true;
	else
		return false;
}
bool CircuitFileParser::LineIsParseableOutput(const std::string& line) {
	if (CountSpaces(line) == 0)
		return true;
	else
		return false;
}

ParsedInput CircuitFileParser::GetParsedInput(const std::string& line) {
	ParsedInput input;
	int substring_start = 0;
	int substring_end = line.find(' ');
	input.node_name = line.substr(substring_start, substring_end - substring_start);
	substring_start = substring_end + 1;
	substring_end = line.find(' ', substring_start);
	input.file_name = line.substr(substring_start, substring_end - substring_start);
	return input;
}
ParsedGate CircuitFileParser::GetParsedGate(const std::string& line) {
	ParsedGate gate;
	int substring_start = 0;
	int substring_end = line.find(' ');
	gate.gate_name = line.substr(substring_start, substring_end - substring_start);
	substring_start = substring_end + 1;
	substring_end = line.find(' ', substring_start);
	gate.ouput_name = line.substr(substring_start, substring_end - substring_start);
	substring_start = substring_end + 1;
	substring_end = line.find(' ', substring_start);
	gate.input1_name = line.substr(substring_start, substring_end - substring_start);
	substring_start = substring_end + 1;
	substring_end = line.find(' ', substring_start);
	gate.input2_name = line.substr(substring_start, substring_end - substring_start);
	return gate;
}
ParsedOutput CircuitFileParser::GetParsedOutput(const std::string& line) {
	ParsedOutput output;
	output.node_name = line;
	return output;
}

CircuitFileParser::~CircuitFileParser() {
}
