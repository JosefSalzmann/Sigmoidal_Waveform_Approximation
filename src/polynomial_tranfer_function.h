/*
*   Polynomial transfer function class
*/
#ifndef POLYNOMIAL_TRANSFER_FUNCTION_H
#define POLYNOMIAL_TRANSFER_FUNCTION_H

#include "datatypes.h"

class PolynomialTranferFunction : public TranferFunction {
   private:
	/* data */
   public:
	PolynomialTranferFunction();
	~PolynomialTranferFunction();
	void ReadModel(const std::string& file_path) override;
	TransitionParameters CalculatePropagation(TransitionParameters input_parameters) override;
};

#endif