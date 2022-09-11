
#include "polynomial_sis_transfer_function.h"

#include <math.h>

#include <cerrno>
#include <fstream>
#include <iostream>

/*
 * Read the coefficients of the polynomial.
 * Not a robust impelementation!
 * Only intedended to read automatically generated transfer functions.
 */
void PolynomialSISTransferFunction::ReadModel(const std::string& file_name) {
	std::ifstream input_file_stream(file_name);
	if (input_file_stream.fail()) {
		std::cerr << "File not found: " << file_name << std::endl;
		throw std::exception();
	}
	model_degree = GetDegreeOfModel(file_name);
	int current_parameter = 0;  // 0 = steepness, 1 = shift
	std::string line = "";
	while (getline(input_file_stream, line)) {
		if (line.compare("") == 0) {
			continue;
		}

		auto line_split = GetLineSplit(line);
		int coeff_index = -1;
		if (line_split.size() == 2) {
			coeff_index = 1;
		} else if (line_split.size() == 3) {
			coeff_index = 2;
		} else if (line_split.size() == 1) {
			continue;
		} else {
			std::cerr << "Parsing error in: " << file_name << std::endl;
			throw std::exception();
		}

		char* endptr;
		errno = 0;
		double current_coeff = std::strtod(line_split[coeff_index].c_str(), &endptr);
		if (*endptr != '\0' ||  // error, we didn't consume the entire string
		    errno != 0)         // error, overflow or underflow
		{
			std::cerr << "conversion error in " << file_name << ": " << line << std::endl;
			throw std::exception();
		}

		if (current_parameter == 0) {
			steepness_tf_coeffs.push_back(current_coeff);
			if ((int)steepness_tf_coeffs.size() == 3 * model_degree + 1) {
				current_parameter++;
			}
		} else {
			shift_tf_coeffs.push_back(current_coeff);
			if ((int)shift_tf_coeffs.size() == 3 * model_degree + 1) {
				return;
			}
		}
	}
}

void PolynomialSISTransferFunction::ReadValidRegionFile(const std::string& file_name) {
	return;
}

void PolynomialSISTransferFunction::SetDefaultValues(const TransitionParameters& default_prev_transition, double maximal_shift) {
	MAX_TIME_SHIFT = maximal_shift;
	default_prev_tr = default_prev_transition;
}

/*
 *  Get the degree of the polynomial.
 *  The first line should contain "output_steepness".
 *  The second line "offset,<offset>", where <offset> is a double.
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
				std::cerr << "Invalid transfer function specification: " << file_name << std::endl;
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
	std::cerr << "Invalid transfer function specification" << file_name << std::endl;
	throw std::exception();
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
	TransitionParameters current_inp_tr = parameters[0];
	TransitionParameters prev_outp_tr;

	// TODO: maybe remove this check and change method signature since we always get two transitions
	if (parameters.size() == 1) {
		prev_outp_tr = default_prev_tr;
	} else {
		prev_outp_tr = parameters[1];
	}

	double current_inp_steep = current_inp_tr.steepness;
	double prev_out_steep = prev_outp_tr.steepness;
	double time_shift = current_inp_tr.shift - prev_outp_tr.shift;

	if (time_shift > MAX_TIME_SHIFT) {
		time_shift = MAX_TIME_SHIFT;
	}

	double output_steep = CalculateParameter(steepness_tf_coeffs, {current_inp_steep, prev_out_steep, time_shift});
	double output_shift = CalculateParameter(shift_tf_coeffs, {current_inp_steep, prev_out_steep, time_shift});

	return {output_steep, current_inp_tr.shift + output_shift};
}

double PolynomialSISTransferFunction::CalculateParameter(
    const std::vector<double>& coeffs,
    const std::vector<double>& parameters) {
	double output_parameter = coeffs[0];
	for (int i = 0; i < (int)parameters.size(); i++) {
		for (int j = 1; j < model_degree + 1; j++) {
			output_parameter += pow(parameters[i], j) * coeffs[i * model_degree + j];
		}
	}
	return output_parameter;
}

void PolynomialSISTransferFunction::SetName(const std::string& transferfunction_name) {
}

PolynomialSISTransferFunction::~PolynomialSISTransferFunction() {
}