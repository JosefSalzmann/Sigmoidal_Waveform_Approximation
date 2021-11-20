#include <iostream>

#include "circuit_file_parser.h"
#include "datatypes.h"

int main() {
	CircuitFileParser circuit_file_parser = CircuitFileParser();
	int ret = circuit_file_parser.ParseFile("../testing/mutliple_gates_same_output_node.txt");
	circuit_file_parser.PerformSanityCheck();
	return ret;
}