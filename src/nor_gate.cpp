
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
 * Mark transitons as canceled if cancelation happens.
 */
void NORGate::PropagateTransition(const std::shared_ptr<Transition>& transition, Input input, const std::shared_ptr<TransitionSchedule>& schedule) {
	bool mis = CheckIfMIS(*transition, input);
	if (mis) {
		// do MIS stuff
		// tbd...
		return;
	}

	// do SIS stuff

	/* if the last transition of the other input was rising i.e. other input is now at
	 * VDD, the transition does not propagate
	 */
	if ((input == Input_A && input_b_transitions.back()->parameters.steepness > 0) ||
	    (input == Input_B && input_a_transitions.back()->parameters.steepness > 0)) {
		return;
	}

	/*
	 * Check for cancellation
	 * If the current transition happens before the most recent output transition,
	 * this transition does not propagate and the most recent output transition is canceled.
	 */
	if (output_transitions.back()->parameters.shift > transition->parameters.shift) {
		// TODO
	}

	TransitionParameters generated_outp_tr_params = CaclulateSISParametersAtInput(transition->parameters, input);
	std::shared_ptr<Transition> generated_outp_tr = std::shared_ptr<Transition>(new Transition);
	generated_outp_tr->parameters = generated_outp_tr_params;
	generated_outp_tr->parents = {transition};
	generated_outp_tr->source = std::shared_ptr<TransitionSource>(shared_from_this());
	generated_outp_tr->sinks = subscribers;

	schedule->AddFutureTransition(generated_outp_tr);
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

/*
 *	Check if transition if the other input also had a transition near to this
 * 	one. If yes, use MIS transfer functions to calculate the output transition.
 */
bool NORGate::CheckIfMIS(const Transition& transition, Input input) {
	double current_tr_shift = transition.parameters.shift;
	if (input == Input_A) {
		auto latest_b_tr = input_b_transitions.back();
		double latest_b_tr_shift = latest_b_tr->parameters.shift;
		// TODO make the 1.0 configurable
		if (current_tr_shift - latest_b_tr_shift < 1.0) {
			return true;
		}
	} else {
		auto latest_a_tr = input_a_transitions.back();
		double latest_a_tr_shift = latest_a_tr->parameters.shift;
		// TODO make the 1.0 configurable
		if (current_tr_shift - latest_a_tr_shift < 1.0) {
			return true;
		}
	}
	return false;
}

NORGate::~NORGate() {
}