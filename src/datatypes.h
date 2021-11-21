/*
*   Global datatypes 
*/
#ifndef DATATYPES_H
#define DATATYPES_H

#include <memory>
#include <string>
#include <vector>

#include "nor_gate.h"

struct ParsedInput {
	std::string node_name;
	std::string file_name;
};

struct ParsedGate {
	std::string gate_name;
	std::string ouput_name;
	std::string input_a_name;
	std::string input_b_name;
	// std::string gate_type;
};

struct ParsedOutput {
	std::string node_name;
};

struct TransitionParameters {
	double shift;
	double steepness;
};

struct NORGateInput;

struct Transition {
	std::string source;               // TODO: use other datatype
	std::vector<NORGateInput> sinks;  // TODO: use other datatype
	TransitionParameters parameters;
	std::unique_ptr<Transition> parent;  // TODO: maybe shared_ptr needs to be used?
	std::vector<std::unique_ptr<Transition>> children;
	bool cancelation;
};

class TranferFunction {
   public:
	virtual ~TranferFunction() {}
	virtual void ReadModel(std::string file_path) = 0;
	virtual TransitionParameters CalculatePropagation(TransitionParameters input_parameters) = 0;
};

#endif