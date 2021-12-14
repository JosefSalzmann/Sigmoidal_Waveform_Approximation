
#include "nor_gate.h"

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

bool NORGate::NORGateSorter(const NORGate& lhs, const NORGate& rhs) {
	return lhs.gate_name < rhs.gate_name;
}

InitialValue NORGate::GetInitialOutputValue() {
	return initial_value_output;
}

/*
 * Propgate a transition and register the newly created transition in the schedule.
 * Mark transitons as cancelled if cancelation happens.
 */
void NORGate::PropagateTransition(const std::shared_ptr<Transition>& transition, Input input, const std::shared_ptr<TransitionSchedule>& schedule) {
	if (output_node_name.compare("OA_4") == 0 && transition->parameters.shift > 70) {
		int debug = 0;
	}
	if (transition->cancelation) {
		return;
	}

	if (input == Input_A) {
		input_a_transitions.push_back(transition);
	} else {
		input_b_transitions.push_back(transition);
	}

	TransitionParameters generated_outp_tr_params;
	std::shared_ptr<Transition> generated_outp_tr = std::shared_ptr<Transition>(new Transition);
	bool mis = CheckIfMIS(transition, input);
	if (mis) {
		// do MIS stuff
		// throw away the last output transition, since it gets overwritten now.
		output_transitions.pop_back();

		generated_outp_tr_params = CaclulateMISParameters(transition->parameters);
		generated_outp_tr->parents = {transition, mis_partner};
		generated_outp_tr->is_MIS = true;
	} else {
		// do SIS stuff

		/* if the last transition of the other input was rising i.e. other input is now at
		* VDD, the transition does not propagate
		*/
		if ((input == Input_A && input_b_transitions.back()->parameters.steepness > 0) ||
		    (input == Input_B && input_a_transitions.back()->parameters.steepness > 0)) {
			return;
		}

		// TODO: find better way to make sure that the last output transition was not cancelled
		while (output_transitions.back()->cancelation) {
			output_transitions.pop_back();
		}

		/*
		* Check for cancelation
		* If the current transition happens before the most recent output transition,
		* this transition does not propagate and the most recent output transition is cancelled.
		*/
		if (output_transitions.back()->parameters.shift + 0.00 > transition->parameters.shift) {
			CancelTransition(output_transitions.back(), schedule);
			output_transitions.pop_back();
			return;
		}

		generated_outp_tr_params = CaclulateSISParametersAtInput(transition->parameters, input);
		generated_outp_tr->parents = {transition};
	}

	// TODO: add check if the genereated output parameters are reasonable!
	generated_outp_tr->parameters = generated_outp_tr_params;
	generated_outp_tr->source = std::shared_ptr<TransitionSource>(shared_from_this());
	generated_outp_tr->sinks = subscribers;
	transition->children.push_back(std::shared_ptr<Transition>(generated_outp_tr));

	schedule->AddFutureTransition(generated_outp_tr);
	output_transitions.push_back(generated_outp_tr);
}

/*
 *	Calculate the output parameters of the output transition cause by current_input_tr
 * 	by using the appropriate SIS transfer function, i.e. right input and right polarity.
 */
TransitionParameters NORGate::CaclulateSISParametersAtInput(TransitionParameters current_input_tr, Input input) {
	TransitionParameters prev_outp_tr = output_transitions.back()->parameters;
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

TransitionParameters NORGate::CaclulateMISParameters(TransitionParameters current_input_tr) {
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
		     output_transitions.back()->parameters});
	} else {
		return transfer_functions->mis_input_b_first_rr->CalculatePropagation(
		    {input_a_params,
		     input_b_params,
		     output_transitions.back()->parameters});
	}
}

/*
 *	Check if transition of the other input also had a transition near to this
 * 	one. If yes, use MIS transfer functions to calculate the output transition.
 */
bool NORGate::CheckIfMIS(const std::shared_ptr<Transition>& transition, Input input) {
	if (transition->parameters.steepness < 0) {
		// TODO: until now only support MIS for both input rising
		return false;
	}
	double current_tr_shift = transition->parameters.shift;
	auto latest_output_tr = output_transitions.back();
	if (input == Input_A) {
		auto latest_b_tr = input_b_transitions.back();
		double latest_b_tr_shift = latest_b_tr->parameters.shift;
		// TODO make the 1.0 configurable
		if (current_tr_shift - latest_b_tr_shift < 1.0 && TransitionsSamePolarity(transition, latest_b_tr) && latest_output_tr->parents[0] == latest_b_tr) {
			mis_partner = latest_b_tr;
			mis_parnter_input = Input_B;
			return true;
		}
	} else {
		auto latest_a_tr = input_a_transitions.back();
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
 * Mark a transition and all its children as cancelled. If a transition gets cancelled that was 
 * the part of a MIS propagation, we have to reschedule the other transitions of the MIS propgagation
 * that did not get cancelled.
 */
void NORGate::CancelTransition(const std::shared_ptr<Transition>& transition, const std::shared_ptr<TransitionSchedule>& schedule) {
	transition->cancelation = true;
	for (auto it = transition->children.begin(); it != transition->children.end(); it++) {
		if ((*it)->is_MIS) {
			// reschedule the parent transition which is not cancelled
			if (!(*it)->parents.front()->cancelation) {
				schedule->AddFutureTransition((*it)->parents.front());
			} else if (!(*it)->parents.back()->cancelation) {
				schedule->AddFutureTransition((*it)->parents.back());
			}
		}
		CancelTransition(*it, schedule);
	}
}
