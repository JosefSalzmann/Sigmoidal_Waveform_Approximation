
#include "polynomial_mis_transfer_function.h"

#include <fstream>
#include <iostream>

PolynomialMISTransferFunction::PolynomialMISTransferFunction(/* args */) {
}

void PolynomialMISTransferFunction::ReadModel(const std::string& file_name) {
}

/*
 * MIS Transfer function: three TransitionParameters as input, namely current input parameters of input a and b and the parameters 
 * of the previous output transition
 */
TransitionParameters PolynomialMISTransferFunction::CalculatePropagation(const std::vector<TransitionParameters>& parameters) {
	return {};
}

PolynomialMISTransferFunction::~PolynomialMISTransferFunction() {
}