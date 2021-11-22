#include <iostream>

#include "circuit_simulator.h"
#include "datatypes.h"

int main() {
	CircuitSimulator simulator = CircuitSimulator();
	simulator.InitializeCircuit("../testing/intial_values_test.txt");
	// system("touch test.asdf");
	return 0;
}