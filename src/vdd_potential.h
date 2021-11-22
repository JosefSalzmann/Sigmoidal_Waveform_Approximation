/*
*   VDD potential class
*/
#ifndef VDD_POTENTIAL_H
#define VDD_POTENTIAL_H

#include "nor_gate.h"

// not really a transitionsource, only needed for determining initial values
class VDDPotential : public TransitionSource {
   private:
	std::vector<NORGateInput> subscribers;

   public:
	VDDPotential() : subscribers{} {};
	~VDDPotential();
	std::string GetOutputName() override;
	InitialValue GetInitialOutputValue() override;
	std::vector<NORGateInput>& GetSubscribers() override;
};
#endif