
#include "polynomial_mis_transfer_function.h"

#include <fstream>
#include <iostream>

PolynomialMISTransferFunction::PolynomialMISTransferFunction(/* args */) {
}

void PolynomialMISTransferFunction::ReadModel(const std::string& file_name) {
	// std::ifstream input_file_stream(file_name);
	// if (input_file_stream.fail()) {
	// 	std::cerr << "File not found: " << file_name << std::endl;
	// 	throw std::exception();
	// }
	// std::string line = "";
	// while (getline(input_file_stream, line)) {
	// 	if (line.compare("") == 0) {
	// 		return;
	// 	}
	// }
}

/*
 * MIS Transfer function: three TransitionParameters as input, namely current input parameters of input a and b and the parameters 
 * of the previous output transition
 */
TransitionParameters PolynomialMISTransferFunction::CalculatePropagation(std::vector<TransitionParameters> parameters) {
	return {};
}

PolynomialMISTransferFunction::~PolynomialMISTransferFunction() {
}