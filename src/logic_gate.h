/*
*   Logic Gate class
*/
#ifndef LOGIC_GATE_H
#define LOGIC_GATE_H

#include <float.h>

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

enum FunctionType {
	NOR,
	INV
};

enum Input {
	Input_A,
	Input_B
};

enum ANNSISTYPE {
	AFALLING,
	ARISING,
	BFALLING,
	BRISING,
	INVERTERFALLING,
	INVERTERRISING
};

class LogicGate;

struct NORGateInput {
	std::shared_ptr<LogicGate> nor_gate;
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
	bool cancelation = false;
	bool is_responsible_for_cancelation = false;
	std::shared_ptr<Transition> cancels_tr;  // transition that gets canceled by this transition
};

enum TFModelType {
	SIS,
	MIS
};

class TransferFunction {
   public:
	virtual ~TransferFunction() {}
	virtual void ReadModel(const std::string& file_name) = 0;
	virtual void ReadBoundaryFile(const std::string& file_name) = 0;
	virtual TransitionParameters CalculatePropagation(const std::vector<TransitionParameters>& parameters) = 0;
	virtual void SetDefaultValues(const TransitionParameters& default_prev_transition, double maximal_shift) = 0;
};

struct TFCollection {
	std::shared_ptr<TransferFunction> sis_input_a_falling;
	std::shared_ptr<TransferFunction> sis_input_a_rising;
	std::shared_ptr<TransferFunction> sis_input_b_falling;
	std::shared_ptr<TransferFunction> sis_input_b_rising;
	std::shared_ptr<TransferFunction> inverter_falling;
	std::shared_ptr<TransferFunction> inverter_rising;
};

#include "gnd_potential.h"
#include "vdd_potential.h"

class LogicGate : public TransitionSource, public std::enable_shared_from_this<LogicGate> {
   private:
	FunctionType gate_type;
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
	std::shared_ptr<Transition> default_falling_tr;
	std::shared_ptr<Transition> default_rising_tr;

	void CancelTransition(const std::shared_ptr<Transition>& transition, const std::shared_ptr<TransitionSchedule>& schedule);
	TransitionParameters CaclulateSISParametersAtInput(TransitionParameters prev_outp_tr, TransitionParameters current_input_tr, Input input);
	TransitionParameters CaclulateMISParameters(TransitionParameters prev_outp_tr, TransitionParameters current_input_tr);
	double CalculatePulseValue(double vdd, double x, TransitionParameters transition1, TransitionParameters transition2);
	bool CheckCancelation(TransitionParameters transition1, TransitionParameters transition2);

   public:
	LogicGate(){};
	LogicGate(const FunctionType gate_type,
	          const std::string& gate_name,
	          const std::string& output_node_name,
	          const std::shared_ptr<TFCollection>& transfer_functions,
	          double default_falling_steepness,
	          double default_rising_steepness) : gate_type{gate_type},
	                                             gate_name{gate_name},
	                                             output_node_name{output_node_name},
	                                             input_a_transitions{},
	                                             input_b_transitions{},
	                                             output_transitions{},
	                                             initial_value_input_a{UNDEFINED},
	                                             initial_value_input_b{UNDEFINED},
	                                             initial_value_output{UNDEFINED},
	                                             subscribers{},
	                                             transfer_functions{transfer_functions},
	                                             default_falling_tr{std::make_shared<Transition>()},
	                                             default_rising_tr{std::make_shared<Transition>()} {
		default_falling_tr->parameters.steepness = default_falling_steepness;
		default_falling_tr->parameters.shift = -DBL_MAX;
		default_rising_tr->parameters.steepness = default_rising_steepness;
		default_rising_tr->parameters.shift = -DBL_MAX;
	};
	std::string GetOutputName() override;
	void SetInitialInput(InitialValue initial_value, Input input);
	void SetInputSource(const std::shared_ptr<TransitionSource>& input_source, Input input);
	std::vector<NORGateInput>& GetSubscribers() override;
	InitialValue GetInitialOutputValue() override;
	std::vector<std::shared_ptr<Transition>>& GetOutputTransitions();
	void AddSubscriber(NORGateInput subscriber);
	void RemoveOutputTransition(std::shared_ptr<Transition> transition);
	void RemoveInputTransition(std::shared_ptr<Transition> transition, Input input);
	void PropagateTransition(const std::shared_ptr<Transition>& transition, Input input, const std::shared_ptr<TransitionSchedule>& schedule);
	void PropagateTransitionNOR(const std::shared_ptr<Transition>& transition, Input input, const std::shared_ptr<TransitionSchedule>& schedule);
	void PropagateTransitionINV(const std::shared_ptr<Transition>& transition, Input input, const std::shared_ptr<TransitionSchedule>& schedule);
	std::shared_ptr<TransitionSource>& GetInputSource(Input input);
	FunctionType GetType();
};

#endif