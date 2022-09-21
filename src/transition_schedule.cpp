/*
  File: transition_schedule.cpp
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

#include "transition_schedule.h"

#include <algorithm>
TransitionSchedule::TransitionSchedule(/* args */) {
}

TransitionSchedule::~TransitionSchedule() {
}

void TransitionSchedule::AddFutureTransition(std::shared_ptr<Transition> transition) {
	if (future_transitions.size() == 0) {
		future_transitions.push_back(transition);
		return;
	}
	if (future_transitions.back()->parameters.shift > transition->parameters.shift) {
		future_transitions.push_back(transition);
		return;
	}
	if (future_transitions.front()->parameters.shift < transition->parameters.shift) {
		future_transitions.insert(future_transitions.begin(), transition);
		return;
	}
	for (auto it = future_transitions.end(); it != future_transitions.begin();) {
		--it;
		if ((*it)->parameters.shift > transition->parameters.shift) {
			future_transitions.insert(it + 1, transition);
			return;
		}
	}
}

void TransitionSchedule::AddPastTransition(std::shared_ptr<Transition> transition) {
	past_transitions.push_back(transition);
}

/*
 * Sorting is done in reverse, i.e. last transition to be scheduled is the first element.
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

/**
 * @brief Check if a transition is scheduled as future transitoin
 *
 * @param transition
 * @return true
 * @return false
 */
bool TransitionSchedule::TransitionIsScheduled(const std::shared_ptr<Transition>& transition) {
	return std::find(future_transitions.begin(), future_transitions.end(), transition) != future_transitions.end();
}