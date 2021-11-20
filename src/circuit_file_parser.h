/*
*   Parser for circuit files
*/
#ifndef CIRCUIT_FILE_PARSER_H
#define CIRCUIT_FILE_PARSER_H

#include <string>
#include <vector>

#include "datatypes.h"

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