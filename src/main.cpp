#include <iostream>

#include "circuit_simulator.h"
#include "datatypes.h"

int main() {
	CircuitSimulator simulator = CircuitSimulator();
	simulator.InitializeCircuit("../testing/gate_parsing_test.txt");
	return 0;
}