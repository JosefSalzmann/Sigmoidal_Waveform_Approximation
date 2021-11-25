#include <iostream>

#include "circuit_simulator.h"

int main() {
	CircuitSimulator simulator = CircuitSimulator();
	try {
		simulator.InitializeCircuit("../testing/test_data/initial_values_test_oscillation.txt");
	} catch (const std::exception& e) {
		return -1;
	}

	return 0;
}