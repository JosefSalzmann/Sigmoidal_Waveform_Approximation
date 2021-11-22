#include <iostream>

#include "circuit_simulator.h"
#include "datatypes.h"

int main() {
	CircuitSimulator simulator = CircuitSimulator();
	simulator.InitializeCircuit("../testing/intial_ values_test.txt");
	return 0;
}