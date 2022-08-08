/*
*   GND potential class
*/
#ifndef GND_POTENTIAL_H
#define GND_POTENTIAL_H

#include "logic_gate.h"

// not really a transitionsource, only needed for determining initial values
class GNDPotential : public TransitionSource {
   private:
	std::vector<NORGateInput> subscribers;

   public:
	GNDPotential() : subscribers{} {};
	~GNDPotential();
	std::string GetOutputName() override;
	InitialValue GetInitialOutputValue() override;
	std::vector<NORGateInput>& GetSubscribers() override;
};
#endif