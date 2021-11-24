/*
*   Polynomial SIS transfer function class
*/
#ifndef POLYNOMIAL_SIS_TRANSFER_FUNCTION_H
#define POLYNOMIAL_SIS_TRANSFER_FUNCTION_H

#include "nor_gate.h"

class PolynomialSISTransferFunction : public TransferFunction {
   private:
	/* data */
   public:
	PolynomialSISTransferFunction();
	~PolynomialSISTransferFunction();
	void ReadModel(const std::string& file_name) override;
	TransitionParameters CalculatePropagation(std::vector<TransitionParameters> parameters) override;
};

#endif