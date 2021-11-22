#include "gnd_potential.h"

std::string GNDPotential::GetOutputName() {
	return "GND";
}

InitialValue GNDPotential::GetInitialOutputValue() {
	return GND;
}

std::vector<NORGateInput>& GNDPotential::GetSubscribers() {
	return subscribers;
}

GNDPotential::~GNDPotential() {}