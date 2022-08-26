/*
 *   Transition Schedule class
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