
#include "polynomial_tranfer_function.h"

#include <fstream>
#include <iostream>

PolynomialTranferFunction::PolynomialTranferFunction(/* args */) {
}

void PolynomialTranferFunction::ReadModel(const std::string& file_name) {
	std::ifstream input_file_stream(file_name);
	if (input_file_stream.fail()) {
		std::cerr << "File not found: " << file_name << std::endl;
		throw std::exception();
	}
	std::string line = "";
	while (getline(input_file_stream, line)) {
		if (line.compare("") == 0) {
			return;
		}
	}
}

TransitionParameters PolynomialTranferFunction::CalculatePropagation(TransitionParameters input_parameters) {
	return {};
}

PolynomialTranferFunction::~PolynomialTranferFunction() {
}