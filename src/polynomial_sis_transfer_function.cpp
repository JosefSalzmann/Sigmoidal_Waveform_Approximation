
#include <fstream>
#include <iostream>

#include "polynomial_sis_transfer_function.h"

PolynomialSISTransferFunction::PolynomialSISTransferFunction(/* args */) {
}

void PolynomialSISTransferFunction::ReadModel(const std::string& file_name) {
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

/*
 * SIS Transfer function: two TransitionParameters as input, namely current input parameters and the parameters 
 * of the previous output transition
 */
TransitionParameters PolynomialSISTransferFunction::CalculatePropagation(std::vector<TransitionParameters> parameters) {
	return {};
}

PolynomialSISTransferFunction::~PolynomialSISTransferFunction() {
}