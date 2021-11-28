/*
*   Polynomial MIS transfer function class
*/
#ifndef POLYNOMIAL_MIS_TRANSFER_FUNCTION_H
#define POLYNOMIAL_MIS_TRANSFER_FUNCTION_H

#include "nor_gate.h"

class PolynomialMISTransferFunction : public TransferFunction {
   private:
   public:
	PolynomialMISTransferFunction();
	~PolynomialMISTransferFunction();
	void ReadModel(const std::string& file_name) override;
	TransitionParameters CalculatePropagation(const std::vector<TransitionParameters>& parameters) override;
	void SetDefaultValues(const TransitionParameters& default_prev_transition, double maximal_shift) override;
};

#endif