/*
 * Interpretation.h
 *
 *  Created on: May 2, 2021
 *      Author:
 */

#ifndef INTERPRETATION_H_
#define INTERPRETATION_H_

#include <string>

class Interpretation {
public:
	static const char* const interp;
	static const char* const function;
	static const std::string closing;

	static void read_model(const std::string&, int** model);
	static void parse_row(const std::string& line, int* vec);
};



#endif /* INTERPRETATION_H_ */
