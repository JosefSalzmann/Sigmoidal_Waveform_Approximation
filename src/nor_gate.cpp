
#include "nor_gate.h"

#include <math.h>

#include <iostream>
#include <string>

void NORGate::SetInputSource(const std::shared_ptr<TransitionSource>& input_source, Input input) {
	if (input == Input_A) {
		input_a_source = input_source;
	} else if (input == Input_B) {
		input_b_source = input_source;
	}
}

std::shared_ptr<TransitionSource>& NORGate::GetInputSource(Input input) {
	if (input == Input_A) {
		return input_a_source;
	} else {
		return input_b_source;
	}
}

std::vector<std::shared_ptr<Transition>>& NORGate::GetOutputTransitions() {
	return output_transitions;
}

void NORGate::AddSubscriber(NORGateInput subscriber) {
	subscribers.push_back(subscriber);
}

void NORGate::SetInitialInput(InitialValue initial_value, Input input) {
	// set initial input value
	if (input == Input_A) {
		initial_value_input_a = initial_value;
		// place a default transition at -double_max
		if (initial_value_input_a == GND) {
			input_a_transitions = {default_falling_tr};
		} else {
			input_a_transitions = {default_rising_tr};
		}
	} else if (input == Input_B) {
		initial_value_input_b = initial_value;
		// place a default transition at -double_max
		if (initial_value_input_b == GND) {
			input_b_transitions = {default_falling_tr};
		} else {
			input_b_transitions = {default_rising_tr};
		}
	}

	// compute new initial output value
	if (initial_value_input_a == UNDEFINED || initial_value_input_b == UNDEFINED) {
		initial_value_output = UNDEFINED;
	} else if (!(initial_value_input_a == VDD || initial_value_input_b == VDD)) {
		initial_value_output = VDD;
		output_transitions = {default_rising_tr};
	} else {
		initial_value_output = GND;
		output_transitions = {default_falling_tr};
	}
}

std::vector<NORGateInput>& NORGate::GetSubscribers() {
	return subscribers;
}

std::string NORGate::GetOutputName() {
	return output_node_name;
}

InitialValue NORGate::GetInitialOutputValue() {
	return initial_value_output;
}

/*
 * Propgate a transition and register the newly created transition in the schedule.
 * Mark transitons as canceled if cancelation happens.
 */
void NORGate::PropagateTransition(const std::shared_ptr<Transition>& transition, Input input, const std::shared_ptr<TransitionSchedule>& schedule) {
	if (output_node_name.compare("OA_9") == 0 && transition->parameters.shift > 102) {
		int debug = 0;
	}

	if (transition->cancelation) {
		return;
	}

	std::cout << "Received Transition: " << std::to_string(transition->parameters.steepness) << "," << std::to_string(transition->parameters.shift)
	          << " at Gate " << this->gate_name << "." << std::endl;

	std::shared_ptr<Transition> latest_valid_input_a_tr;
	std::shared_ptr<Transition> latest_valid_input_b_tr;
	std::shared_ptr<Transition> latest_valid_output_tr;
	std::shared_ptr<Transition> second_latest_valid_output_tr;

	for (auto it = input_a_transitions.rbegin(); it != input_a_transitions.rend(); it++) {
		if (!(*it)->cancelation) {
			latest_valid_input_a_tr = *it;
			break;
		}
	}

	for (auto it = input_b_transitions.rbegin(); it != input_b_transitions.rend(); it++) {
		if (!(*it)->cancelation) {
			latest_valid_input_b_tr = *it;
			break;
		}
	}

	bool latest_v_out_tr_found = false;
	for (auto it = output_transitions.rbegin(); it != output_transitions.rend(); it++) {
		if (!(*it)->cancelation) {
			if (!latest_v_out_tr_found) {
				latest_valid_output_tr = *it;
				latest_v_out_tr_found = true;
			} else {
				second_latest_valid_output_tr = *it;
				break;
			}
		}
	}

	if (input == Input_A) {
		input_a_transitions.push_back(transition);
	} else {
		input_b_transitions.push_back(transition);
	}

	TransitionParameters generated_outp_tr_params;
	std::shared_ptr<Transition> generated_outp_tr = std::shared_ptr<Transition>(new Transition);
	bool mis = CheckIfMIS(transition, latest_valid_output_tr, latest_valid_input_a_tr, latest_valid_input_b_tr, input);
	if (mis) {
		// do MIS stuff
		// throw away the last output transition, since it gets overwritten now.

		//output_transitions.pop_back();

		generated_outp_tr_params = CaclulateMISParameters(second_latest_valid_output_tr->parameters, transition->parameters);
		generated_outp_tr->parents = {transition, mis_partner};
		generated_outp_tr->is_MIS = true;
	} else {
		// do SIS stuff

		/* if the last transition of the other input was rising i.e. other input is now at
		* VDD, the transition does not propagate
		*/
		if ((input == Input_A && latest_valid_input_b_tr->parameters.steepness > 0) ||
		    (input == Input_B && latest_valid_input_a_tr->parameters.steepness > 0)) {
			return;
		}

		// TODO: find better way to make sure that the last output transition was not canceled
		// while (output_transitions.back()->cancelation) {
		// 	output_transitions.pop_back();
		// }

		generated_outp_tr_params = CaclulateSISParametersAtInput(latest_valid_output_tr->parameters, transition->parameters, input);
		generated_outp_tr->parents = {transition};
	}

	// TODO: add check if the genereated output parameters are reasonable!
	generated_outp_tr->parameters = generated_outp_tr_params;
	generated_outp_tr->source = std::shared_ptr<TransitionSource>(shared_from_this());
	generated_outp_tr->sinks = subscribers;
	transition->children.push_back(std::shared_ptr<Transition>(generated_outp_tr));

	/*
	* Check for cancelation
	*/
	if (CheckCancelation(latest_valid_output_tr->parameters, generated_outp_tr_params)) {
		if (output_node_name.compare("OA_1") == 0 && transition->parameters.shift > 100) {
			int debug = 0;
		}
		std::cout << "Would generate Transition: " << std::to_string(generated_outp_tr_params.steepness) << "," << std::to_string(generated_outp_tr_params.shift)
		          << " at Gate " << this->gate_name << ". but gets canceled" << std::endl;
		CancelTransition(latest_valid_output_tr, schedule);
		generated_outp_tr->cancelation = true;
		generated_outp_tr->is_responsible_for_cancelation = true;
		generated_outp_tr->cancels_tr = latest_valid_output_tr;
	} else {
		std::cout << "Generatred Transition: " << std::to_string(generated_outp_tr_params.steepness) << "," << std::to_string(generated_outp_tr_params.shift)
		          << " at Gate " << this->gate_name << "." << std::endl;
	}

	schedule->AddFutureTransition(generated_outp_tr);
	output_transitions.push_back(generated_outp_tr);
}

/*
 *	Calculate the output parameters of the output transition cause by current_input_tr
 * 	by using the appropriate SIS transfer function, i.e. right input and right polarity.
 */
TransitionParameters NORGate::CaclulateSISParametersAtInput(TransitionParameters prev_outp_tr, TransitionParameters current_input_tr, Input input) {
	if (input == Input_A) {
		if (current_input_tr.steepness > 0) {
			// rising transition at input A
			return transfer_functions->sis_input_a_rising->CalculatePropagation(
			    {current_input_tr, prev_outp_tr});
		} else {
			// falling transition at input A
			return transfer_functions->sis_input_a_falling->CalculatePropagation(
			    {current_input_tr, prev_outp_tr});
		}
	} else {
		if (current_input_tr.steepness > 0) {
			// rising transition at input B
			return transfer_functions->sis_input_b_rising->CalculatePropagation(
			    {current_input_tr, prev_outp_tr});
		} else {
			// falling transition at input B
			return transfer_functions->sis_input_b_falling->CalculatePropagation(
			    {current_input_tr, prev_outp_tr});
		}
	}
}

TransitionParameters NORGate::CaclulateMISParameters(TransitionParameters prev_outp_tr, TransitionParameters current_input_tr) {
	TransitionParameters input_a_params, input_b_params;
	if (mis_parnter_input == Input_A) {
		input_a_params = mis_partner->parameters;
		input_b_params = current_input_tr;
	} else {
		input_a_params = current_input_tr;
		input_b_params = mis_partner->parameters;
	}

	if (input_a_params.shift < input_b_params.shift) {
		return transfer_functions->mis_input_a_first_rr->CalculatePropagation(
		    {input_a_params,
		     input_b_params,
		     prev_outp_tr});
	} else {
		return transfer_functions->mis_input_b_first_rr->CalculatePropagation(
		    {input_a_params,
		     input_b_params,
		     prev_outp_tr});
	}
}

/*
 *	Check if transition of the other input also had a transition near to this
 * 	one. If yes, use MIS transfer functions to calculate the output transition.
 */
bool NORGate::CheckIfMIS(const std::shared_ptr<Transition>& transition,
                         const std::shared_ptr<Transition>& latest_output_tr,
                         const std::shared_ptr<Transition>& latest_a_tr,
                         const std::shared_ptr<Transition>& latest_b_tr, Input input) {
	if (transition->parameters.steepness < 0) {
		// TODO: until now only support MIS for both input rising
		return false;
	}
	double current_tr_shift = transition->parameters.shift;
	if (input == Input_A) {
		double latest_b_tr_shift = latest_b_tr->parameters.shift;
		// TODO make the 1.0 configurable
		if (current_tr_shift - latest_b_tr_shift < 1.0 && TransitionsSamePolarity(transition, latest_b_tr) && latest_output_tr->parents[0] == latest_b_tr) {
			mis_partner = latest_b_tr;
			mis_parnter_input = Input_B;
			return true;
		}
	} else {
		double latest_a_tr_shift = latest_a_tr->parameters.shift;
		// TODO make the 1.0 configurable
		if (current_tr_shift - latest_a_tr_shift < 1.0 && TransitionsSamePolarity(transition, latest_a_tr) && latest_output_tr->parents[0] == latest_a_tr) {
			mis_partner = latest_a_tr;
			mis_parnter_input = Input_A;
			return true;
		}
	}
	return false;
}

/*
 *	Return true if both transitions are falling or both are rising, false otherwise.
 */
bool NORGate::TransitionsSamePolarity(const std::shared_ptr<Transition>& transition1, const std::shared_ptr<Transition>& transition2) {
	if ((transition1->parameters.steepness > 0 && transition2->parameters.steepness > 0) ||
	    (transition1->parameters.steepness <= 0 && transition2->parameters.steepness <= 0)) {
		return true;
	} else {
		return false;
	}
}

/*
 * Mark a transition and all its children as canceled. If a transition gets canceled that was 
 * the part of a MIS propagation, we have to reschedule the other transitions of the MIS propgagation
 * that did not get canceled.
 */
void NORGate::CancelTransition(const std::shared_ptr<Transition>& transition, const std::shared_ptr<TransitionSchedule>& schedule) {
	std::cout << "Canceled Transition: " << std::to_string(transition->parameters.steepness) << "," << std::to_string(transition->parameters.shift)
	          << " at Gate " << transition->source->GetOutputName() << "." << std::endl;
	if (transition->is_responsible_for_cancelation) {
		transition->cancels_tr->cancelation = false;
	}
	transition->cancelation = true;
	for (auto it = transition->children.begin(); it != transition->children.end(); it++) {
		if ((*it)->is_MIS) {
			// reschedule the parent transition which is not canceled
			if (!(*it)->parents.front()->cancelation) {
				schedule->AddFutureTransition((*it)->parents.front());
			} else if (!(*it)->parents.back()->cancelation) {
				schedule->AddFutureTransition((*it)->parents.back());
			}
		}
		CancelTransition(*it, schedule);
	}
}

/*
 * Calculate the function value of two adjacent transitions.
 */
double NORGate::CalculatePulseValue(double vdd, double x, TransitionParameters transition1, TransitionParameters transition2) {
	double time_mult = pow(10, 10);
	double tr1_value = 1 / (1 + exp(-transition1.steepness * (time_mult * x - transition1.shift)));
	double tr2_value = 1 / (1 + exp(-transition2.steepness * (time_mult * x - transition2.shift)));
	double correction_value = (transition1.steepness > 0) ? -1 : 0;
	return vdd * (tr1_value + tr2_value + correction_value);
}

/*
 * Check if two transitions are so close to each other that they cancel each other.
 * Checks by looking at the maximum/minimum value generated by the addition of the
 * two transitions. If the maximum/minimum is smaller/greater than Vdd/2 they cancel each other.
 */
bool NORGate::CheckCancelation(TransitionParameters transition1, TransitionParameters transition2) {
	if (abs(transition1.shift - transition2.shift) < 0.2) {
		return true;
	}
	double vdd = 1.2;
	int n_points = 20;
	double test_points[n_points];

	double correction_value = pow(10, -10);
	double start_time = transition1.shift * correction_value;
	double time_step = (transition2.shift * correction_value - start_time) / (double)n_points;

	for (int i = 0; i < n_points; i++) {
		test_points[i] = CalculatePulseValue(vdd, start_time + (double)i * time_step, transition1, transition2);
	}

	if (transition1.steepness > 0) {  //search for the maximum value of the testpoints
		double max_value = -INFINITY;
		for (int i = 0; i < n_points; i++) {
			if (test_points[i] > max_value) {
				max_value = test_points[i];
			}
		}
		if (max_value < vdd / 4) {
			return true;
		}
	} else {  //search for the minimum value of the testpoints
		double min_value = INFINITY;
		for (int i = 0; i < n_points; i++) {
			if (test_points[i] < min_value) {
				min_value = test_points[i];
			}
		}
		if (min_value > vdd * 3 / 4) {
			return true;
		}
	}
	return false;
}
