/*
  File: ann_sis_transfer_function.h
  Author: Josef Salzmann <josef.salzmann@tuwien.ac.at>
  Copyright (c) 2022

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
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