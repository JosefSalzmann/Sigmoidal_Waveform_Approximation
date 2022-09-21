/*
  File: circuit_file_parser.h
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

#ifndef CIRCUIT_FILE_PARSER_H
#define CIRCUIT_FILE_PARSER_H

#include <vector>

#include "logic_gate.h"

struct ParsedInput {
	std::string node_name;
	std::string file_name;
};

struct ParsedGate {
	std::string gate_type;
	std::string gate_name;
	std::string ouput_name;
	std::string input_a_name;
	std::string input_b_name;
};

struct ParsedOutput {
	std::string node_name;
};

struct ParsedTFModel {
	std::string tf_model_type;  // possible values: SIS_Input_A_Falling, SIS_Input_B_Rising, SIS_Inverter_...
	std::string tf_approach;    // possible values: LUT, POLY = Polyonimal, ANN = Artificial Neural Network, ...
	std::string file_name;
};

class CircuitFileParser {
   private:
	std::vector<ParsedInput> parsed_inputs;
	std::vector<ParsedGate> parsed_gates;
	std::vector<ParsedOutput> parsed_outputs;
	std::vector<ParsedTFModel> unordered_parsed_tf_models;
	std::vector<ParsedTFModel> ordered_parsed_tf_models;
	std::string path_to_circuit_file;

   public:
	CircuitFileParser() : parsed_inputs{}, parsed_gates{}, parsed_outputs{}, unordered_parsed_tf_models{}, ordered_parsed_tf_models{} {}
	~CircuitFileParser();
	int ParseFile(const std::string& file_name);
	bool PerformSanityCheck();
	std::vector<ParsedInput>& GetInputs();
	std::vector<ParsedGate>& GetGates();
	std::vector<ParsedOutput>& GetOutputs();
	std::vector<ParsedTFModel>& GetTFModels();

   private:
	int CountSpaces(const std::string& line);
	bool LineIsParseableInput(const std::string& line);
	bool LineIsParseableGate(const std::string& line);
	bool LineIsParseableOutput(const std::string& line);
	bool LineIsParseableTFModel(const std::string& line);
	ParsedInput GetParsedInput(const std::string& line);
	ParsedGate GetParsedGate(const std::string& line);
	ParsedOutput GetParsedOutput(const std::string& line);
	ParsedTFModel GetParsedTFModel(const std::string& line);
	void SetPathToCircuitFile(const std::string& file_path);
	void OrderTransferFunctions();
	bool CompareTFModel(const ParsedTFModel& parsed_tf_model, const std::string& tf_model_type, const std::string& tf_approach);
};

#endif