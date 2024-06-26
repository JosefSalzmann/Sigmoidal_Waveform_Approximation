/*
  File: logic_gate.cpp
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

#include "logic_gate.h"

#include <math.h>
#include <plog/Log.h>

#include <iostream>
#include <string>

void LogicGate::SetInputSource(const std::shared_ptr<TransitionSource>& input_source, Input input) {
	if (input == Input_A) {
		input_a_source = input_source;
	} else if (input == Input_B) {
		input_b_source = input_source;
	}
}

std::shared_ptr<TransitionSource>& LogicGate::GetInputSource(Input input) {
	if (input == Input_A) {
		return input_a_source;
	} else {
		return input_b_source;
	}
}

std::vector<std::shared_ptr<Transition>>& LogicGate::GetOutputTransitions() {
	return output_transitions;
}

void LogicGate::AddSubscriber(NORGateInput subscriber) {
	subscribers.push_back(subscriber);
}

void LogicGate::SetInitialInput(InitialValue initial_value, Input input) {
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

	if (gate_type == INV) {
		if (initial_value == VDD) {
			output_transitions = {default_falling_tr};
		} else {
			output_transitions = {default_rising_tr};
		}
	}
}

std::vector<NORGateInput>& LogicGate::GetSubscribers() {
	return subscribers;
}

std::string LogicGate::GetOutputName() {
	return output_node_name;
}

InitialValue LogicGate::GetInitialOutputValue() {
	return initial_value_output;
}

void LogicGate::PropagateTransition(const std::shared_ptr<Transition>& transition, Input input, const std::shared_ptr<TransitionSchedule>& schedule) {
	if (gate_type == NOR) {
		return PropagateTransitionNOR(transition, input, schedule);
	} else if (gate_type == INV) {
		return PropagateTransitionINV(transition, input, schedule);
	}
}

/*
 * Propgate a transition and register the newly created transition in the schedule.
 * Mark transitons as canceled if cancelation happens.
 */
void LogicGate::PropagateTransitionNOR(const std::shared_ptr<Transition>& transition, Input input, const std::shared_ptr<TransitionSchedule>& schedule) {
	if (transition->cancelation) {
		return;
	}

	PLOG_DEBUG_IF(logging) << "Received Transition: " << std::to_string(transition->parameters.steepness) << "," << std::to_string(transition->parameters.shift)
	                       << " at Gate " << this->gate_name << ".";

	std::shared_ptr<Transition> latest_valid_input_a_tr;
	std::shared_ptr<Transition> latest_valid_input_b_tr;
	std::shared_ptr<Transition> latest_valid_output_tr;

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

	for (auto it = output_transitions.rbegin(); it != output_transitions.rend(); it++) {
		if (!(*it)->cancelation) {
			latest_valid_output_tr = *it;
			break;
		}
	}

	PLOG_DEBUG_IF(logging) << "Latest Valid Output Transition: " << std::to_string(latest_valid_output_tr->parameters.steepness) << "," << std::to_string(latest_valid_output_tr->parameters.shift)
	                       << " at Gate " << this->gate_name << ".";

	if (input == Input_A) {
		input_a_transitions.push_back(transition);
	} else {
		input_b_transitions.push_back(transition);
	}

	TransitionParameters generated_outp_tr_params;
	std::shared_ptr<Transition> generated_outp_tr = std::shared_ptr<Transition>(new Transition);

	// do SIS stuff

	/* if the last transition of the other input was rising i.e. other input is now at
	 * VDD, the transition does not propagate
	 */
	if ((input == Input_A && latest_valid_input_b_tr->parameters.steepness > 0) ||
	    (input == Input_B && latest_valid_input_a_tr->parameters.steepness > 0)) {
		if (input == Input_A) {
			latest_valid_input_b_tr->cancels_tr = transition;
			latest_valid_input_b_tr->is_responsible_for_cancelation = true;
		} else {
			latest_valid_input_a_tr->cancels_tr = transition;
			latest_valid_input_a_tr->is_responsible_for_cancelation = true;
		}
		std::string input_name = (input == Input_A) ? "Input A" : "Input B";
		double latest_steepness = (input == Input_A) ? latest_valid_input_b_tr->parameters.steepness : latest_valid_input_a_tr->parameters.steepness;
		double latest_shift = (input == Input_A) ? latest_valid_input_b_tr->parameters.shift : latest_valid_input_a_tr->parameters.shift;
		PLOG_DEBUG_IF(logging) << "No propagation from " << input_name << ", latest other input transition was (" << std::to_string(latest_steepness) << "," << std::to_string(latest_shift) << ").";
		return;
	} else if ((input == Input_A && latest_valid_input_b_tr->parameters.steepness < 0) ||
	           (input == Input_B && latest_valid_input_a_tr->parameters.steepness < 0)) {
		if (latest_valid_output_tr->parameters.steepness * transition->parameters.steepness < 0) {
			PLOG_DEBUG_IF(logging) << "Output already consistent with inputs (something bad happended).";
			return;
		}
	}

	generated_outp_tr_params = CaclulateSISParametersAtInput(latest_valid_output_tr->parameters, transition->parameters, input);
	generated_outp_tr->direct_parents = {transition};
	if (input == Input_A) {
		generated_outp_tr->indirect_parents = {latest_valid_input_b_tr};
		latest_valid_input_b_tr->indirect_children.push_back(std::shared_ptr<Transition>(generated_outp_tr));
	} else if (input == Input_B) {
		generated_outp_tr->indirect_parents = {latest_valid_input_a_tr};
		latest_valid_input_a_tr->indirect_children.push_back(std::shared_ptr<Transition>(generated_outp_tr));
	}

	generated_outp_tr->parameters = generated_outp_tr_params;
	generated_outp_tr->source = std::shared_ptr<TransitionSource>(shared_from_this());
	generated_outp_tr->sinks = subscribers;
	transition->direct_children.push_back(std::shared_ptr<Transition>(generated_outp_tr));

	/*
	 * Check for cancelation
	 */
	if (CheckCancelation(latest_valid_output_tr->parameters, generated_outp_tr_params)) {
		PLOG_DEBUG_IF(logging) << "Would generate Transition: " << std::to_string(generated_outp_tr_params.steepness) << "," << std::to_string(generated_outp_tr_params.shift)
		                       << " at Gate " << this->gate_name << ".";
		CancelTransition(latest_valid_output_tr, schedule);
		generated_outp_tr->cancelation = true;
		generated_outp_tr->is_responsible_for_cancelation = true;
		generated_outp_tr->cancels_tr = latest_valid_output_tr;
	} else {
		PLOG_DEBUG_IF(logging) << "Generatred Transition: " << std::to_string(generated_outp_tr_params.steepness) << "," << std::to_string(generated_outp_tr_params.shift)
		                       << " at Gate " << this->gate_name << ".";
	}

	schedule->AddFutureTransition(generated_outp_tr);
	output_transitions.push_back(generated_outp_tr);
}

void LogicGate::PropagateTransitionINV(const std::shared_ptr<Transition>& transition, Input input, const std::shared_ptr<TransitionSchedule>& schedule) {
	if (transition->cancelation) {
		return;
	}

	std::shared_ptr<Transition> latest_valid_output_tr;
	std::shared_ptr<Transition> second_latest_valid_output_tr;

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

	PLOG_DEBUG_IF(logging) << "Received Transition: " << std::to_string(transition->parameters.steepness) << "," << std::to_string(transition->parameters.shift)
	                       << " at Gate " << this->gate_name << "."
	                       << " Latest Valid Output Transition is: " << std::to_string(latest_valid_output_tr->parameters.steepness);

	input_a_transitions.push_back(transition);

	TransitionParameters generated_outp_tr_params;
	std::shared_ptr<Transition> generated_outp_tr = std::shared_ptr<Transition>(new Transition);
	generated_outp_tr_params = CaclulateSISParametersAtInput(latest_valid_output_tr->parameters, transition->parameters, input);
	generated_outp_tr->direct_parents = {transition};
	generated_outp_tr->indirect_parents = {};

	generated_outp_tr->parameters = generated_outp_tr_params;
	generated_outp_tr->source = std::shared_ptr<TransitionSource>(shared_from_this());
	generated_outp_tr->sinks = subscribers;
	transition->direct_children.push_back(std::shared_ptr<Transition>(generated_outp_tr));

	/*
	 * Check for cancelation
	 */
	if (CheckCancelation(latest_valid_output_tr->parameters, generated_outp_tr_params)) {
		PLOG_DEBUG_IF(logging) << "Would generate Transition: " << std::to_string(generated_outp_tr_params.steepness) << "," << std::to_string(generated_outp_tr_params.shift)
		                       << " at Gate " << this->gate_name << ".";
		CancelTransition(latest_valid_output_tr, schedule);
		generated_outp_tr->cancelation = true;
		generated_outp_tr->is_responsible_for_cancelation = true;
		generated_outp_tr->cancels_tr = latest_valid_output_tr;
	} else {
		PLOG_DEBUG_IF(logging) << "Generatred Transition: " << std::to_string(generated_outp_tr_params.steepness) << "," << std::to_string(generated_outp_tr_params.shift)
		                       << " at Gate " << this->gate_name << ".";
	}

	schedule->AddFutureTransition(generated_outp_tr);
	output_transitions.push_back(generated_outp_tr);
}

/*
 *	Calculate the output parameters of the output transition cause by current_input_tr
 * 	by using the appropriate SIS transfer function, i.e. right input and right polarity.
 */
TransitionParameters LogicGate::CaclulateSISParametersAtInput(TransitionParameters prev_outp_tr, TransitionParameters current_input_tr, Input input) {
	if (gate_type == NOR) {
		if (this->subscribers.size() == 1) {
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
		} else {
			// use FO2 transfer_functions
			if (input == Input_A) {
				if (current_input_tr.steepness > 0) {
					// rising transition at input A
					return transfer_functions->sis_input_a_FO2_rising->CalculatePropagation(
					    {current_input_tr, prev_outp_tr});
				} else {
					// falling transition at input A
					return transfer_functions->sis_input_a_FO2_falling->CalculatePropagation(
					    {current_input_tr, prev_outp_tr});
				}
			} else {
				if (current_input_tr.steepness > 0) {
					// rising transition at input B
					return transfer_functions->sis_input_b_FO2_rising->CalculatePropagation(
					    {current_input_tr, prev_outp_tr});
				} else {
					// falling transition at input B
					return transfer_functions->sis_input_b_FO2_falling->CalculatePropagation(
					    {current_input_tr, prev_outp_tr});
				}
			}
		}
	} else {  //(gate_type == INV)
		if (current_input_tr.steepness > 0) {
			return transfer_functions->inverter_rising->CalculatePropagation(
			    {current_input_tr, prev_outp_tr});
		} else {
			return transfer_functions->inverter_falling->CalculatePropagation(
			    {current_input_tr, prev_outp_tr});
		}
	}
}

/*
 * Mark a transition and all its children as canceled. If a transition gets canceled that was
 * the part of a MIS propagation, we have to reschedule the other transitions of the MIS propgagation
 * that did not get canceled.
 */
void LogicGate::CancelTransition(const std::shared_ptr<Transition>& transition, const std::shared_ptr<TransitionSchedule>& schedule) {
	PLOG_DEBUG_IF(logging) << "Canceled Transition: " << std::to_string(transition->parameters.steepness) << "," << std::to_string(transition->parameters.shift)
	                       << " at Output " << transition->source->GetOutputName() << ".";

	transition->cancelation = true;

	for (auto it = transition->direct_children.begin(); it != transition->direct_children.end(); it++) {
		CancelTransition(*it, schedule);
	}

	for (auto it = transition->sinks.begin(); it != transition->sinks.end(); it++) {
		it->nor_gate->CheckInputOutputConsistenty(transition->parameters.shift, schedule);
	}
}

/*
 * Mark sure that input and output are consistent after cancelation happened at an input.
 * Generates and output transition if needed.
 */
void LogicGate::CheckInputOutputConsistenty(double time, const std::shared_ptr<TransitionSchedule>& schedule) {
	assert(this->gate_type == NOR);

	std::shared_ptr<Transition> latest_valid_input_a_tr;
	std::shared_ptr<Transition> latest_valid_input_b_tr;
	std::shared_ptr<Transition> latest_valid_output_tr;

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
				break;
			}
		}
	}

	TransitionParameters consistency_outp_tr_params;
	if (latest_valid_input_a_tr->parameters.steepness < 0 && latest_valid_input_b_tr->parameters.steepness < 0) {
		if (latest_valid_output_tr->parameters.steepness > 0) {
			// input and output are consistent, nothing to do
			return;
		} else {
			consistency_outp_tr_params = {100, time};
		}
	} else {
		if (latest_valid_output_tr->parameters.steepness < 0) {
			// input and output are consistent, nothing to do
			return;
		} else {
			consistency_outp_tr_params = {-100, time};
		}
	}

	std::shared_ptr<Transition> consistency_outp_tr = std::shared_ptr<Transition>(new Transition);
	consistency_outp_tr->parameters = consistency_outp_tr_params;
	consistency_outp_tr->source = std::shared_ptr<TransitionSource>(shared_from_this());
	consistency_outp_tr->sinks = this->subscribers;

	schedule->AddFutureTransition(consistency_outp_tr);
	output_transitions.push_back(consistency_outp_tr);

	PLOG_DEBUG_IF(logging) << "Added Consistency Transition: " << std::to_string(consistency_outp_tr_params.steepness) << "," << std::to_string(consistency_outp_tr_params.shift)
	                       << " at Gate " << this->gate_name << ".";
}

/*
 * Calculate the function value of two adjacent transitions.
 */
double LogicGate::CalculatePulseValue(double vdd, double x, TransitionParameters transition1, TransitionParameters transition2) {
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
bool LogicGate::CheckCancelation(TransitionParameters transition1, TransitionParameters transition2) {
	if (transition1.steepness * transition2.steepness > 0) {
		// both same sign - this must not happen
		PLOG_DEBUG_IF(logging) << "Same-Sign Violation at " << this->gate_name;
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

	if (transition1.steepness > 0) {  // search for the maximum value of the testpoints
		double max_value = -INFINITY;
		for (int i = 0; i < n_points; i++) {
			if (test_points[i] > max_value) {
				max_value = test_points[i];
			}
		}
		if (max_value < vdd / 2) {
			return true;
		}
	} else {  // search for the minimum value of the testpoints
		double min_value = INFINITY;
		for (int i = 0; i < n_points; i++) {
			if (test_points[i] < min_value) {
				min_value = test_points[i];
			}
		}
		if (min_value > vdd / 2) {
			return true;
		}
	}
	return false;
}

FunctionType LogicGate::GetType() {
	return gate_type;
}