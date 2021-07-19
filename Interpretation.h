/*
 * Interpretation.h
 *
 *  Created on: May 2, 2021
 *      Author:
 */

#ifndef INTERPRETATION_H_
#define INTERPRETATION_H_

#include <istream>
#include <sstream>
#include <string>
#include <vector>

enum InterpretationType {
	function = 0, relation = 1, unary_function = 2, ternary_function = 3
};

class Interpretation {
public:
	static const std::string interp;
	static const std::string function;
	static const std::string relation;
	static const std::string binary_op;
	static const std::string ternary_op;
	static const std::string unary_op;
	static const std::string closing;
	static const std::string nested_closing;

	static void parse_row(const std::string& line, int* vec);
	static void count_appearances(const std::string& line, int* vec);
	static int  parse_interpretation(std::istream& is, int domain_size, std::vector<int**>& all_mt,
			std::stringstream& ss, std::vector<int>& op_type, std::vector<std::string>& op_sym, bool extract = true);
};



#endif /* INTERPRETATION_H_ */
