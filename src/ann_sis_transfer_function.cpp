#include "ann_sis_transfer_function.h"

#include <iostream>

#include "/usr/lib/cppflow/include/cppflow/model.h"
#include "/usr/lib/cppflow/include/cppflow/ops.h"

void ANNSISTransferFunction::ReadModel(const std::string& file_name) {
	auto input = cppflow::tensor({2.0126f, 6.9102f, 7.2325f});
	cppflow::model model(file_name);
	auto output = model({{"serving_default_normalization_input:0", input}}, {"StatefulPartitionedCall:0"});

	auto values = output.back().get_data<float>();

	for (auto v : values) {
		std::cout << v << std::endl;
	}
}

TransitionParameters ANNSISTransferFunction::CalculatePropagation(const std::vector<TransitionParameters>& parameters) {
	return {};
}
void ANNSISTransferFunction::SetDefaultValues(const TransitionParameters& default_prev_transition, double maximal_shift) {
}