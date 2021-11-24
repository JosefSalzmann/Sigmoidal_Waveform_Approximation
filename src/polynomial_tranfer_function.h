/*
*   Polynomial transfer function class
*/
#ifndef POLYNOMIAL_TRANSFER_FUNCTION_H
#define POLYNOMIAL_TRANSFER_FUNCTION_H

#include "nor_gate.h"

class PolynomialTranferFunction : public TranferFunction {
   private:
	/* data */
   public:
	PolynomialTranferFunction();
	~PolynomialTranferFunction();
	void ReadModel(const std::string& file_name) override;
	TransitionParameters CalculatePropagation(TransitionParameters input_parameters) override;
};

#endif