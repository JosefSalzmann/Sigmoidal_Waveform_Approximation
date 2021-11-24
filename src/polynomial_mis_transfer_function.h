/*
*   Polynomial MIS transfer function class
*/
#ifndef POLYNOMIAL_MIS_TRANSFER_FUNCTION_H
#define POLYNOMIAL_MIS_TRANSFER_FUNCTION_H

#include "nor_gate.h"

class PolynomialMISTransferFunction : public TransferFunction {
   private:
	/* data */
   public:
	PolynomialMISTransferFunction();
	~PolynomialMISTransferFunction();
	void ReadModel(const std::string& file_name) override;
	TransitionParameters CalculatePropagation(std::vector<TransitionParameters> parameters) override;
};

#endif