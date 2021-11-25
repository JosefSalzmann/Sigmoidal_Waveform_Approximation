/*
*   Polynomial SIS transfer function class
*/
#ifndef POLYNOMIAL_SIS_TRANSFER_FUNCTION_H
#define POLYNOMIAL_SIS_TRANSFER_FUNCTION_H

#include "nor_gate.h"

class PolynomialSISTransferFunction : public TransferFunction {
   private:
	std::vector<double> steepness_tf_coeffs;
	std::vector<double> shift_tf_coeffs;

	int GetDegreeOfModel(const std::string& file_name);
	std::vector<std::string> GetLineSplit(const std::string& line);

   public:
	PolynomialSISTransferFunction();
	~PolynomialSISTransferFunction();
	void ReadModel(const std::string& file_name) override;
	TransitionParameters CalculatePropagation(const std::vector<TransitionParameters>& parameters) override;
};

#endif