
#include "nor_gate.h"

#include "datatypes.h"

NORGate::~NORGate() {
}

std::string NORGate::GetName() {
	return gate_name;
}

InitialValue NORGate::GetInitialOutput() {
	return initial_output;
}

void NORGate::SetInitialInput(InitialValue initial_value, Input input) {
}