/*
*   NOR Gate class
*/
#ifndef NOR_GATE_H
#define NOR_GATE_H

#include <memory>
#include <string>
#include <vector>

#include "transition_schedule.h"

class TransitionSchedule;

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
	double steepness;
	double shift;
};

struct Transition {
	std::shared_ptr<TransitionSource> source;
	std::vector<NORGateInput> sinks;
	TransitionParameters parameters;
	std::vector<std::shared_ptr<Transition>> parents;  // one parent for SIS, two parents for MIS
	std::vector<std::shared_ptr<Transition>> children;
	bool cancelation;
};

enum TFModelType {
	SIS,
	MIS
};

class TransferFunction {
   public:
	virtual ~TransferFunction() {}
	virtual void ReadModel(const std::string& file_name) = 0;
	virtual TransitionParameters CalculatePropagation(const std::vector<TransitionParameters>& parameters) = 0;
	virtual void SetDefaultValues(const TransitionParameters& default_prev_transition, double maximal_shift) = 0;
};

struct TFCollection {
	std::shared_ptr<TransferFunction> sis_input_a_falling;
	std::shared_ptr<TransferFunction> sis_input_a_rising;
	std::shared_ptr<TransferFunction> sis_input_b_falling;
	std::shared_ptr<TransferFunction> sis_input_b_rising;
	std::shared_ptr<TransferFunction> mis_input_a_first_rr;  // both inputs rising, shift_a < shift_b
	std::shared_ptr<TransferFunction> mis_input_b_first_rr;  // both inputs rising, shift_b < shift_a
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
	std::shared_ptr<TFCollection> transfer_functions;

   public:
	NORGate(){};
	NORGate(const std::string& gate_name,
	        const std::string& output_node_name,
	        const std::shared_ptr<TFCollection>& transfer_functions) : gate_name{gate_name},
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
	std::shared_ptr<Transition> PropagateTransition(const Transition&, Input input, const std::shared_ptr<TransitionSchedule>& schedule);
	static bool NORGateSorter(const NORGate& lhs, const NORGate& rhs);
	std::shared_ptr<TransitionSource>& GetInputSource(Input input);
};

#endif