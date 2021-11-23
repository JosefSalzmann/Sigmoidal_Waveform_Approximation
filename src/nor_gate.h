/*
*   NOR Gate class
*/
#ifndef NOR_GATE_H
#define NOR_GATE_H

#include <memory>
#include <string>
#include <vector>

struct NORGateInput;
struct Transition;

enum InitialValue {
	GND,
	VDD,
	UNDEFINED
};

enum Input {
	Input_A,
	Input_B
};

class NORGate;

struct NORGateInput {
	std::shared_ptr<NORGate> nor_gate;
	Input input;
};

class TransitionSource {
   public:
	virtual ~TransitionSource() {}
	virtual std::string GetOutputName() = 0;
	virtual InitialValue GetInitialOutputValue() = 0;
	virtual std::vector<NORGateInput>& GetSubscribers() = 0;
};

struct TransitionParameters {
	double shift;
	double steepness;
};

struct Transition {
	std::shared_ptr<TransitionSource> source;
	std::vector<NORGateInput> sinks;
	TransitionParameters parameters;
	std::unique_ptr<Transition> parent;  // TODO: maybe shared_ptr needs to be used?
	std::vector<std::unique_ptr<Transition>> children;
	bool cancelation;
};

class TranferFunction {
   public:
	virtual ~TranferFunction() {}
	virtual void ReadModel(const std::string& file_path) = 0;
	virtual TransitionParameters CalculatePropagation(TransitionParameters input_parameters) = 0;
};

#include "gnd_potential.h"
#include "vdd_potential.h"

class NORGate : public TransitionSource {
   private:
	std::string gate_name;
	std::string output_node_name;
	std::shared_ptr<TransitionSource> input_a_source;
	std::shared_ptr<TransitionSource> input_b_source;
	std::vector<std::shared_ptr<Transition>> input_a_transitions;
	std::vector<std::shared_ptr<Transition>> input_b_transitions;
	std::vector<std::shared_ptr<Transition>> output_transitions;
	InitialValue initial_value_input_a;
	InitialValue initial_value_input_b;
	InitialValue initial_value_output;
	std::vector<NORGateInput> subscribers;
	double transfer_functions;  // TODO: use right datatype

   public:
	NORGate(){};
	NORGate(const std::string& gate_name,
	        const std::string& output_node_name,
	        double transfer_functions) : gate_name{gate_name},
	                                     output_node_name{output_node_name},
	                                     initial_value_input_a{UNDEFINED},
	                                     initial_value_input_b{UNDEFINED},
	                                     initial_value_output{UNDEFINED},
	                                     subscribers{},
	                                     transfer_functions{transfer_functions} {};
	~NORGate();
	std::string GetOutputName() override;
	void SetInitialInput(InitialValue initial_value, Input input);
	void SetInputSource(const std::shared_ptr<TransitionSource>& input_source, Input input);
	std::vector<NORGateInput>& GetSubscribers() override;
	InitialValue GetInitialOutputValue() override;
	std::vector<std::shared_ptr<Transition>>& GetOutputTransitions();
	void AddSubscriber(NORGateInput subscriber);
	void RemoveOutputTransition(std::shared_ptr<Transition> transition);
	void RemoveInputTransition(std::shared_ptr<Transition> transition, Input input);
	std::vector<std::shared_ptr<Transition>> PropagateTransition(std::shared_ptr<Transition> transition, Input input);
	static bool NORGateSorter(const NORGate& lhs, const NORGate& rhs);
	std::shared_ptr<TransitionSource>& GetInputSource(Input input);
};

#endif