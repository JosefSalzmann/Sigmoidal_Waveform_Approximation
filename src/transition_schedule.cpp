
#include "transition_schedule.h"

#include <algorithm>

TransitionSchedule::TransitionSchedule(/* args */) {
}

TransitionSchedule::~TransitionSchedule() {
}

void TransitionSchedule::AddFutureTransition(std::shared_ptr<Transition> transition) {
	future_transitions.push_back(transition);
}

void TransitionSchedule::AddPastTransition(std::shared_ptr<Transition> transition) {
	past_transitions.push_back(transition);
}

/*
 * Sorting is done in reverse, i.e. last transition to scheduled is the first element.
 */
void TransitionSchedule::SortFutureTransitions() {
	std::sort(future_transitions.begin(), future_transitions.end(), &TransitionSorter);
}

bool TransitionSchedule::HasFutureTransitions() {
	return future_transitions.size() > 0;
}

std::shared_ptr<Transition> TransitionSchedule::ConsumeFirstTransition() {
	auto first_transition = future_transitions.back();
	future_transitions.pop_back();
	return first_transition;
}

bool TransitionSchedule::TransitionSorter(const std::shared_ptr<Transition>& lhs, const std::shared_ptr<Transition>& rhs) {
	return lhs->parameters.shift > rhs->parameters.shift;
}