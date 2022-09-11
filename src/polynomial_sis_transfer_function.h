/*
 *   Polynomial SIS transfer function class
 */
#ifndef POLYNOMIAL_SIS_TRANSFER_FUNCTION_H
#define POLYNOMIAL_SIS_TRANSFER_FUNCTION_H

#include "logic_gate.h"

class PolynomialSISTransferFunction : public TransferFunction {
   private:
	std::vector<double> steepness_tf_coeffs;
	std::vector<double> shift_tf_coeffs;
	TransitionParameters default_prev_tr;
	double MAX_TIME_SHIFT;
	int model_degree;

	int GetDegreeOfModel(const std::string& file_name);
	std::vector<std::string> GetLineSplit(const std::string& line);
	double CalculateParameter(const std::vector<double>& coeffs, const std::vector<double>& parameters);

   public:
	PolynomialSISTransferFunction() : default_prev_tr{}, MAX_TIME_SHIFT{1.5}, model_degree{-1} {};
	~PolynomialSISTransferFunction();
	void ReadModel(const std::string& file_name) override;
	void ReadValidRegionFile(const std::string& file_name) override;
	TransitionParameters CalculatePropagation(const std::vector<TransitionParameters>& parameters) override;
	void SetDefaultValues(const TransitionParameters& default_prev_transition, double maximal_shift) override;
	void SetName(const std::string& transferfunction_name) override;
};

#endif