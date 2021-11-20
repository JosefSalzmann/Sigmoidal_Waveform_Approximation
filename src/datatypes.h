/*
*   Global datatypes 
*/
#ifndef DATATYPES_H
#define DATATYPES_H

#include <memory>
#include <string>
#include <vector>

struct ParsedInput {
	std::string node_name;
	std::string file_name;
};

struct ParsedGate {
	std::string gate_name;
	std::string ouput_name;
	std::string input1_name;
	std::string input2_name;
	// std::string gate_type;
};

struct ParsedOutput {
	std::string node_name;
};

enum InitialValue {
	GND,
	VDD,
	UNDEFINED
};

enum Input {
	Input_A,
	Input_B
};

struct Transition {
	std::string source;              // TODO: use other datatype
	std::vector<std::string> sinks;  // TODO: use other datatype
	double shift;
	double steepness;
	std::unique_ptr<Transition> parent;  // TODO: maybe shared_ptr needs to be used?
	std::vector<std::unique_ptr<Transition>> children;
	bool cancelation;
};

#endif