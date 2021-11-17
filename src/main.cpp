#include <iostream>

#include "circuit_file_parser.h"

int main() {
	CircuitFileParser circuit_file_parser = CircuitFileParser();
	int ret = circuit_file_parser.ParseFile("test.txt");
	circuit_file_parser.PerformSanityCheck();
	return ret;
}