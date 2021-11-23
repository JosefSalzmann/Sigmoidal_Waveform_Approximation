#include <iostream>

#include "circuit_simulator.h"
#include "datatypes.h"

int main() {
	CircuitSimulator simulator = CircuitSimulator();
	simulator.InitializeCircuit("../testing/test_data/intial_values_test.txt");
	return 0;
}