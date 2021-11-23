
#include "transition_schedule.h"

#include <algorithm>

TransitionSchedule::TransitionSchedule(/* args */) {
}

TransitionSchedule::~TransitionSchedule() {
}

void TransitionSchedule::AddFutureTransition(std::shared_ptr<Transition> transition) {
	future_transitions.push_back(transition);
}

void TransitionSchedule::SortFutureTransitions() {
	std::sort(future_transitions.begin(), future_transitions.end(), &TransitionSorter);
}

bool TransitionSchedule::TransitionSorter(const std::shared_ptr<Transition>& lhs, const std::shared_ptr<Transition>& rhs) {
	return lhs->parameters.shift < rhs->parameters.shift;
}