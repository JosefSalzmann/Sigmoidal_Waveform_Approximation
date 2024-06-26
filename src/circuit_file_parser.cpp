/*
  File: cgal_test.h
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

#include "circuit_file_parser.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <string>

#include "logic_gate.h"

int CircuitFileParser::ParseFile(const std::string& file_name) {
	SetPathToCircuitFile(file_name);

	int current_section = -1;  // 0 = input, 1 = gates, 2 = outputs, 3 = transferfunctions
	bool parsing_failed = false;

	std::ifstream input_file_stream(file_name);
	if (input_file_stream.fail()) {
		std::cerr << "File not found: " << file_name << std::endl;
		throw std::exception();
	}
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
			} else if (line_no_front_space.compare(0, 17, "TRANSFERFUNCTIONS") == 0) {
				current_section = 3;
			} else {
				if (current_section == 0) {
					if (LineIsParseableInput(line_no_front_space)) {
						parsed_inputs.push_back(GetParsedInput(line));
					} else {
						std::cerr << "Cannot parse: " << line << std::endl;
						parsing_failed = true;
						break;
					}
				} else if (current_section == 1) {
					if (LineIsParseableGate(line_no_front_space)) {
						parsed_gates.push_back(GetParsedGate(line));
					} else {
						std::cerr << "Cannot parse: " << line << std::endl;
						parsing_failed = true;
						break;
					}
				} else if (current_section == 2) {
					if (LineIsParseableOutput(line_no_front_space)) {
						parsed_outputs.push_back(GetParsedOutput(line));
					} else {
						std::cerr << "Cannot parse: " << line << std::endl;
						parsing_failed = true;
						break;
					}
				} else if (current_section == 3) {
					if (LineIsParseableTFModel(line_no_front_space)) {
						unordered_parsed_tf_models.push_back(GetParsedTFModel(line));
					} else {
						std::cerr << "Cannot parse: " << line << std::endl;
						parsing_failed = true;
						break;
					}
				}
			}
		}
	}
	input_file_stream.close();
	if (parsing_failed) {
		throw std::exception();
	}
	// TODO: check if all sections were present.
	OrderTransferFunctions();
	return 0;
}

/*
 * Return the path to circuit file, i.e. everything of the file_path except the filename.
 */
void CircuitFileParser::SetPathToCircuitFile(const std::string& file_path) {
	int previous_slash_pos = 0;
	auto slash_pos = file_path.find('/');
	while (slash_pos != std::string::npos) {
		previous_slash_pos = slash_pos + 1;
		slash_pos = file_path.find('/', slash_pos + 1);
	}
	path_to_circuit_file = file_path.substr(0, previous_slash_pos);
}

/**
 * Check that each circuit node has exactly one source and atleast one sink.
 * Nodes with more than one source cause abortion.
 * TODO: Maybe cause abortion when a sink has no source?
 * TODO: Check if all gate and input names are different
 */
bool CircuitFileParser::PerformSanityCheck() {
	std::multiset<std::string> source_names, sink_names;

	for (auto it = parsed_inputs.begin(); it != parsed_inputs.end(); it++) {
		source_names.insert(it->node_name);
	}

	for (auto it = parsed_gates.begin(); it != parsed_gates.end(); it++) {
		source_names.insert(it->ouput_name);
		std::string input_a_name = it->input_a_name;
		if (input_a_name.compare("GND") != 0 &&
		    input_a_name.compare("VDD") != 0)
			sink_names.insert(it->input_a_name);
		std::string input_b_name = it->input_b_name;
		if (input_b_name.compare("GND") != 0 &&
		    input_b_name.compare("VDD") != 0)
			sink_names.insert(it->input_b_name);
	}

	for (auto it = parsed_outputs.begin(); it != parsed_outputs.end(); it++) {
		sink_names.insert(it->node_name);
	}

	// look for duplicates in the sink multi set
	auto duplicate = std::adjacent_find(source_names.begin(), source_names.end());
	if (duplicate != source_names.end()) {
		std::cerr << "Circuit node " << *duplicate << " has multiple sources." << std::endl;
		throw std::exception();
	}

	// look for sinks that have no associated source
	for (auto it = sink_names.begin(); it != sink_names.end(); it++) {
		auto source = source_names.find(*it);
		if (source == source_names.end()) {
			std::cerr << "Error, circuit node " << *it << " is not connected." << std::endl;
			return false;
		}
	}
	return true;
}

std::vector<ParsedInput>& CircuitFileParser::GetInputs() {
	return parsed_inputs;
}
std::vector<ParsedGate>& CircuitFileParser::GetGates() {
	return parsed_gates;
}
std::vector<ParsedOutput>& CircuitFileParser::GetOutputs() {
	return parsed_outputs;
}

std::vector<ParsedTFModel>& CircuitFileParser::GetTFModels() {
	return ordered_parsed_tf_models;
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
	if (CountSpaces(line) == 4 or CountSpaces(line) == 3)
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

bool CircuitFileParser::LineIsParseableTFModel(const std::string& line) {
	if (CountSpaces(line) == 2)
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
	std::stringstream ss;
	ss << path_to_circuit_file << line.substr(substring_start, substring_end - substring_start);
	input.file_name = ss.str();
	return input;
}
ParsedGate CircuitFileParser::GetParsedGate(const std::string& line) {
	ParsedGate gate;
	int substring_start = 0;
	int substring_end = line.find(' ');
	gate.gate_type = line.substr(substring_start, substring_end - substring_start);
	substring_start = substring_end + 1;
	substring_end = line.find(' ', substring_start);
	gate.gate_name = line.substr(substring_start, substring_end - substring_start);
	substring_start = substring_end + 1;
	substring_end = line.find(' ', substring_start);
	gate.ouput_name = line.substr(substring_start, substring_end - substring_start);
	substring_start = substring_end + 1;
	substring_end = line.find(' ', substring_start);
	gate.input_a_name = line.substr(substring_start, substring_end - substring_start);
	substring_start = substring_end + 1;
	if (substring_start > (int)line.size()) {
		gate.input_b_name = "";
		return gate;
	}
	substring_end = line.find(' ', substring_start);
	gate.input_b_name = line.substr(substring_start, substring_end - substring_start);
	return gate;
}
ParsedOutput CircuitFileParser::GetParsedOutput(const std::string& line) {
	ParsedOutput output;
	output.node_name = line;
	return output;
}

ParsedTFModel CircuitFileParser::GetParsedTFModel(const std::string& line) {
	ParsedTFModel tf_model;
	int substring_start = 0;
	int substring_end = line.find(' ');
	tf_model.tf_model_type = line.substr(substring_start, substring_end - substring_start);
	substring_start = substring_end + 1;
	substring_end = line.find(' ', substring_start);
	tf_model.tf_approach = line.substr(substring_start, substring_end - substring_start);
	substring_start = substring_end + 1;
	substring_end = line.find(' ', substring_start);
	std::stringstream ss;
	ss << path_to_circuit_file << line.substr(substring_start, substring_end - substring_start);
	tf_model.file_name = ss.str();
	return tf_model;
}

/**
 * @brief Order the parsed transfer functions in a defined way
 */
void CircuitFileParser::OrderTransferFunctions() {
	ParsedTFModel parsed_tf_models_arr[20];
	for (auto it = unordered_parsed_tf_models.begin(); it < unordered_parsed_tf_models.end(); it++) {
		ParsedTFModel current_model = *it;
		if (CompareTFModel(current_model, "SIS_A_F", "ANN")) {
			parsed_tf_models_arr[0] = current_model;
		} else if (CompareTFModel(current_model, "SIS_A_F", "OFF")) {
			parsed_tf_models_arr[1] = current_model;
		} else if (CompareTFModel(current_model, "SIS_A_R", "ANN")) {
			parsed_tf_models_arr[2] = current_model;
		} else if (CompareTFModel(current_model, "SIS_A_R", "OFF")) {
			parsed_tf_models_arr[3] = current_model;
		} else if (CompareTFModel(current_model, "SIS_B_F", "ANN")) {
			parsed_tf_models_arr[4] = current_model;
		} else if (CompareTFModel(current_model, "SIS_B_F", "OFF")) {
			parsed_tf_models_arr[5] = current_model;
		} else if (CompareTFModel(current_model, "SIS_B_R", "ANN")) {
			parsed_tf_models_arr[6] = current_model;
		} else if (CompareTFModel(current_model, "SIS_B_R", "OFF")) {
			parsed_tf_models_arr[7] = current_model;
		} else if (CompareTFModel(current_model, "SIS_A_FO2_F", "ANN")) {
			parsed_tf_models_arr[8] = current_model;
		} else if (CompareTFModel(current_model, "SIS_A_FO2_F", "OFF")) {
			parsed_tf_models_arr[9] = current_model;
		} else if (CompareTFModel(current_model, "SIS_A_FO2_R", "ANN")) {
			parsed_tf_models_arr[10] = current_model;
		} else if (CompareTFModel(current_model, "SIS_A_FO2_R", "OFF")) {
			parsed_tf_models_arr[11] = current_model;
		} else if (CompareTFModel(current_model, "SIS_B_FO2_F", "ANN")) {
			parsed_tf_models_arr[12] = current_model;
		} else if (CompareTFModel(current_model, "SIS_B_FO2_F", "OFF")) {
			parsed_tf_models_arr[13] = current_model;
		} else if (CompareTFModel(current_model, "SIS_B_FO2_R", "ANN")) {
			parsed_tf_models_arr[14] = current_model;
		} else if (CompareTFModel(current_model, "SIS_B_FO2_R", "OFF")) {
			parsed_tf_models_arr[15] = current_model;
		} else if (CompareTFModel(current_model, "Inverter_F", "ANN")) {
			parsed_tf_models_arr[16] = current_model;
		} else if (CompareTFModel(current_model, "Inverter_F", "OFF")) {
			parsed_tf_models_arr[17] = current_model;
		} else if (CompareTFModel(current_model, "Inverter_R", "ANN")) {
			parsed_tf_models_arr[18] = current_model;
		} else if (CompareTFModel(current_model, "Inverter_R", "OFF")) {
			parsed_tf_models_arr[19] = current_model;
		}
	}
	for (int i = 0; i < (int)unordered_parsed_tf_models.size(); i++) {
		ordered_parsed_tf_models.push_back(parsed_tf_models_arr[i]);
	}
}

bool CircuitFileParser::CompareTFModel(const ParsedTFModel& parsed_tf_model, const std::string& tf_model_type, const std::string& tf_approach) {
	return parsed_tf_model.tf_model_type.compare(tf_model_type) == 0 && parsed_tf_model.tf_approach.compare(tf_approach) == 0;
}

CircuitFileParser::~CircuitFileParser() {
}
