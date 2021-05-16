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

class Interpretation {
public:
	static const std::string interp;
	static const std::string function;
	static const std::string binary_op;
	static const std::string closing;
	static const std::string nested_closing;

	static void parse_row(const std::string& line, int* vec);
	static int  parse_interpretation(std::istream& is, int domain_size, std::vector<int**> all_mt, std::stringstream& ss);
};



#endif /* INTERPRETATION_H_ */
