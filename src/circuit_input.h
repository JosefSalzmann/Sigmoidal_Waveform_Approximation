/*
*   Circuit input class
*/
#ifndef CIRCUIT_INPUT_H
#define CIRCUIT_INPUT_H

#include <string>

#include "datatypes.h"
#include "nor_gate.h"

class CircuitInput : public TransitionSource {
   private:
	std::string file_name;
	std::string node_name;
	std::vector<std::shared_ptr<NORGateInput>> subscribers;
	InitialValue initial_value = UNDEFINED;

   public:
	CircuitInput(const std::string& file_name,
	             const std::string& node_name) : file_name{file_name},
	                                             node_name{node_name} {};
	std::vector<std::shared_ptr<NORGateInput>>& GetSubscribers() override;
	void DetermineInitialValue();
	~CircuitInput();
};
#endif