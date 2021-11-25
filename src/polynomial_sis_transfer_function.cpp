
#include "polynomial_sis_transfer_function.h"

#include <cerrno>
#include <fstream>
#include <iostream>

PolynomialSISTransferFunction::PolynomialSISTransferFunction(/* args */) {
}

void PolynomialSISTransferFunction::ReadModel(const std::string& file_name) {
	std::ifstream input_file_stream(file_name);
	if (input_file_stream.fail()) {
		std::cerr << "File not found: " << file_name << std::endl;
		throw std::exception();
	}
	int model_degree = GetDegreeOfModel(file_name);
	std::string line = "";
	while (getline(input_file_stream, line)) {
		if (line.compare("") == 0) {
			continue;
		}
		auto test = GetLineSplit(line);
	}
}

/*
 *  Get the degree of the polynomial.
 *  The first line should contain "output_steepness".
 *  The second line "offset,xxxxxxxxx".
 *  And the following lines the coefficients of the first parameter.
 */
int PolynomialSISTransferFunction::GetDegreeOfModel(const std::string& file_name) {
	std::ifstream input_file_stream(file_name);
	std::string line = "";
	int line_count = 1;
	int max_degree_so_far = 0;
	while (getline(input_file_stream, line)) {
		if (line_count >= 3) {
			auto line_split = GetLineSplit(line);
			if (line_split.size() != 3) {
				std::cerr << "Invalid transfer function specification" << line << std::endl;
				throw std::exception();
			}
			int degree = atoi(line_split[1].c_str());
			// TODO: implement handling for conversion error
			// {
			// 	std::cerr << "conversion error in " << file_name << ": " << line << std::endl;
			// 	throw std::exception();
			// }
			if (degree > max_degree_so_far) {
				max_degree_so_far = degree;
			} else {
				return max_degree_so_far;
			}
		}
		line_count++;
	}
	return 0;
}

/*
 *	Split line into parts seperated by ','
 */
std::vector<std::string> PolynomialSISTransferFunction::GetLineSplit(const std::string& line) {
	std::vector<std::string> line_split;
	int previous_slash_pos = 0;
	auto slash_pos = line.find(',');
	while (slash_pos != std::string::npos) {
		line_split.push_back(line.substr(previous_slash_pos, slash_pos - previous_slash_pos));
		previous_slash_pos = slash_pos + 1;
		slash_pos = line.find(',', previous_slash_pos);
	}
	slash_pos = line.size();
	line_split.push_back(line.substr(previous_slash_pos, slash_pos - previous_slash_pos));
	return line_split;
}

/*
 * SIS Transfer function: two TransitionParameters as input, namely current input parameters and the parameters 
 * of the previous output transition
 */
TransitionParameters PolynomialSISTransferFunction::CalculatePropagation(const std::vector<TransitionParameters>& parameters) {
	return {};
}

PolynomialSISTransferFunction::~PolynomialSISTransferFunction() {
}