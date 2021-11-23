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

class CircuitFileParser {
   private:
	std::vector<ParsedInput> inputs;
	std::vector<ParsedGate> gates;
	std::vector<ParsedOutput> outputs;

   public:
	CircuitFileParser() : inputs{}, gates{}, outputs{} {}
	~CircuitFileParser();
	int ParseFile(const std::string& file_name);
	bool PerformSanityCheck();
	std::vector<ParsedInput>& GetInputs();
	std::vector<ParsedGate>& GetGates();
	std::vector<ParsedOutput>& GetOutputs();
	// std::vector<NORGateInput> GetInputSubscribers(ParsedInput Input);
	// std::vector<NORGateInput> GetGateSubscribers(ParsedGate Gate);

   private:
	int CountSpaces(const std::string& line);
	bool LineIsParseableInput(const std::string& line);
	bool LineIsParseableGate(const std::string& line);
	bool LineIsParseableOutput(const std::string& line);
	ParsedInput GetParsedInput(const std::string& line);
	ParsedGate GetParsedGate(const std::string& line);
	ParsedOutput GetParsedOutput(const std::string& line);
};

#endif