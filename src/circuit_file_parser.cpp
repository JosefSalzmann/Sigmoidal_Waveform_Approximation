/*
*   Parser for circuit files
*/

#include "circuit_file_parser.h"

#include <fstream>
#include <iostream>
#include <string>

using namespace std;

CircuitFileParser::CircuitFileParser() {
}

int CircuitFileParser::ParseFile(const string& file_name) {
	int inputs = 0;
	int gates = 0;
	int outputs = 0;

	ifstream input_file_stream(file_name);
	string line;
	while (getline(input_file_stream, line)) {
		int index_first_car = 0;
		int line_len = line.length();
		// find index of first non space character
		while (index_first_car < (line_len - 1) && (line[index_first_car] == ' ' || line[index_first_car] == '\t')) {
			index_first_car++;
		}
		// line is comment if first non space character is '*',
		// also skip line if it is empty
		if (line[index_first_car] != '*' && index_first_car != line_len)
			cout << line << "\n";
	}
	return 0;
}

CircuitFileParser::~CircuitFileParser() {
}
