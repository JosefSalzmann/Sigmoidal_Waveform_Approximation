/*
 *   Parser for circuit files
 */
#ifndef ANN_SIS_TRANSFER_FUNCTION_H
#define ANN_SIS_TRANSFER_FUNCTION_H

#include <cppflow/model.h>
#include <cppflow/ops.h>

#include "logic_gate.h"
#include "valid_region_containment.h"

class ANNSISTransferFunction : public TransferFunction {
   private:
	std::unique_ptr<cppflow::model> steepness_model;
	std::unique_ptr<cppflow::model> shift_model;
	TransitionParameters default_prev_tr;
	double MAX_TIME_SHIFT;
	int model_degree;
	ANNSISTYPE tf_type;
	ValidRegionContainment valid_region_containment;
	std::string transferfunction_name;
	bool logging;

	int GetDegreeOfModel(const std::string& file_name);
	std::vector<std::string> GetLineSplit(const std::string& line);
	double CalculateParameter(const std::vector<double>& coeffs, const std::vector<double>& parameters);

   public:
	explicit ANNSISTransferFunction(const ANNSISTYPE& tf_type, bool logging) : default_prev_tr{}, MAX_TIME_SHIFT{1.5}, model_degree{-1}, tf_type{tf_type}, logging{logging} {};
	void ReadModel(const std::string& file_name) override;
	void ReadValidRegionFile(const std::string& file_name) override;
	TransitionParameters CalculatePropagation(const std::vector<TransitionParameters>& parameters) override;
	void SetDefaultValues(const TransitionParameters& default_prev_transition, double maximal_shift) override;
	void SetName(const std::string& transferfunction_name) override;
};

#endif