/*
*   NOR Gate class
*/
#ifndef NOR_GATE_H
#define NOR_GATE_H

#include <memory>
#include <string>

#include "datatypes.h"

struct NORGateInput;

class NORGate {
   private:
	std::string gate_name;
	std::string output_node_name;
	std::vector<std::shared_ptr<Transition>> input_a_transitions;
	std::vector<std::shared_ptr<Transition>> input_b_transitions;
	std::vector<std::shared_ptr<Transition>> output_transitions;
	InitialValue initial_input_a;
	InitialValue initial_input_b;
	InitialValue initial_output;
	std::vector<std::shared_ptr<NORGateInput>> subscribers;
	double transfer_functions;  // TODO: use right datatype

   public:
	NORGate(const std::string& gate_name,
	        const std::string& output_node_name,
	        double transfer_functions) : gate_name{gate_name},
	                                     output_node_name{output_node_name},
	                                     initial_input_a{UNDEFINED},
	                                     initial_input_b{UNDEFINED},
	                                     initial_output{VDD},
	                                     transfer_functions{transfer_functions} {};
	~NORGate();
	std::string GetName();
	InitialValue GetInitialOutput();
	void SetInitialInput(InitialValue initial_value, Input input);
	std::vector<std::shared_ptr<NORGateInput>>& GetSubscribers();
	std::string GetOutputNodeName();
	std::vector<std::shared_ptr<Transition>>& GetOutputTransitions();
	void AddSbuscriber(std::shared_ptr<NORGateInput> subscriber);
	void RemoveOutputTransition(std::shared_ptr<Transition> transition);
	void RemoveInputTransition(std::shared_ptr<Transition> transition, Input input);
	std::vector<std::shared_ptr<Transition>> PropagateTransition(std::shared_ptr<Transition> transition, Input input);
};

struct NORGateInput {
	std::unique_ptr<NORGate> nor_gate;
	Input input;
};

#endif