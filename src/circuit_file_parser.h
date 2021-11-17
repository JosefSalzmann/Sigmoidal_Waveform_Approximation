/*
*   Parser for circuit files
*/
#ifndef CIRCUIT_FILE_PARSER_H
#define CIRCUIT_FILE_PARSER_H

#include <string>

class CircuitFileParser {
   private:
	/* data */
   public:
	CircuitFileParser();
	~CircuitFileParser();
	int ParseFile(const std::string& file_name);
};

#endif