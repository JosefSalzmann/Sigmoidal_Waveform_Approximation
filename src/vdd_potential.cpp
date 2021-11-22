#include "vdd_potential.h"

std::string VDDPotential::GetOutputName() {
	return "VDD";
}

InitialValue VDDPotential::GetInitialOutputValue() {
	return VDD;
}

std::vector<NORGateInput>& VDDPotential::GetSubscribers() {
	return subscribers;
}

VDDPotential::~VDDPotential() {}