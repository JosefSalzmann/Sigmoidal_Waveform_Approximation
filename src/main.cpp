#include <iostream>

#include "circuit_simulator.h"

int main() {
	CircuitSimulator simulator = CircuitSimulator();
	try {
		simulator.InitializeCircuit("../testing/test_data/tf_model_test.txt");
	} catch (const std::exception& e) {
		return -1;
	}

	return 0;
}