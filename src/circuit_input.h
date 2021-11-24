/*
*   Circuit input class
*/
#ifndef CIRCUIT_INPUT_H
#define CIRCUIT_INPUT_H

#include <string>

#include "nor_gate.h"

class CircuitInput : public TransitionSource {
   private:
	std::string file_name;
	std::string node_name;
	std::vector<NORGateInput> subscribers;
	InitialValue initial_value = UNDEFINED;
	std::vector<std::shared_ptr<Transition>> transitions;
	TransitionParameters ParseInputFileLine(const std::string& line);

   public:
	CircuitInput(const std::string& file_name,
	             const std::string& node_name) : file_name{file_name},
	                                             node_name{node_name},
	                                             transitions{} {};
	std::vector<NORGateInput>& GetSubscribers() override;
	InitialValue GetInitialOutputValue() override;
	std::string GetOutputName() override;
	void AddSubscriber(NORGateInput subscriber);
	void DetermineInitialValue();
	void ReadTransitionsFromInputFile();
	std::vector<std::shared_ptr<Transition>>& GetTransitions();
	~CircuitInput();
};
#endif