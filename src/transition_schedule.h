/*
  File: transition_schedule.h
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

#ifndef TRANSITION_SCHEDULE_H
#define TRANSITION_SCHEDULE_H
#include <memory>
#include <vector>

#include "logic_gate.h"

struct Transition;

class TransitionSchedule {
   private:
	std::vector<std::shared_ptr<Transition>> future_transitions;
	std::vector<std::shared_ptr<Transition>> past_transitions;

   public:
	TransitionSchedule();
	~TransitionSchedule();
	void AddFutureTransition(std::shared_ptr<Transition> transition);
	void AddPastTransition(std::shared_ptr<Transition> transition);
	void SortFutureTransitions();
	std::shared_ptr<Transition> ConsumeFirstTransition();
	bool HasFutureTransitions();
	void CancelTransition(std::shared_ptr<Transition> first_cancelation_partner, std::shared_ptr<Transition> second_cancelation_partner);
	static bool TransitionSorter(const std::shared_ptr<Transition>& lhs, const std::shared_ptr<Transition>& rhs);
	bool TransitionIsScheduled(const std::shared_ptr<Transition>& transition);
};
#endif