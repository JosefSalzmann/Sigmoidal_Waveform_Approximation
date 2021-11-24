/*
*   Parser for circuit files
*/
#ifndef CIRCUIT_FILE_PARSER_H
#define CIRCUIT_FILE_PARSER_H

#include <vector>

#include "nor_gate.h"

struct ParsedInput {
	std::string node_name;
	std::string file_name;
};

struct ParsedGate {
	std::string gate_name;
	std::string ouput_name;
	std::string input_a_name;
	std::string input_b_name;
	// std::string gate_type;
};

struct ParsedOutput {
	std::string node_name;
};

struct ParsedTFModel {
	std::string tf_model_type;  // possible values: SIS_Input_A_Falling, SIS_Input_B_Rising, MIS_Input_A_First_RR
	std::string tf_approach;    // possible values: LUT, POLY = Polyonimal, ANN = Artificial Neural Network, ...
	std::string file_name;
};

class CircuitFileParser {
   private:
	std::vector<ParsedInput> parsed_inputs;
	std::vector<ParsedGate> parsed_gates;
	std::vector<ParsedOutput> parsed_outputs;
	std::vector<ParsedTFModel> parsed_tf_models;

   public:
	CircuitFileParser() : parsed_inputs{}, parsed_gates{}, parsed_outputs{} {}
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
};

#endif