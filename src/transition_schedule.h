/*
*   Transition Schedule class
*/
#ifndef TRANSITION_SCHEDULE_H
#define TRANSITION_SCHEDULE_H
#include <memory>

#include "datatypes.h"

class TransitionSchedule {
   private:
	std::vector<Transition> future_transitions;
	std::vector<Transition> past_transitions;

   public:
	TransitionSchedule();
	~TransitionSchedule();
	void AddFutureTransition(Transition transition);
	void AddPastTransition(Transition transition);
	std::shared_ptr<Transition> ConsumeFirstTransition();
	bool HasFutureTransitions();
	void CancelTransition(std::shared_ptr<Transition> first_cancelation_partner, std::shared_ptr<Transition> second_cancelation_partner);
};
#endif