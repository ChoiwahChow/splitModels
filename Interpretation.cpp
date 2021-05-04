/*
 * Interpretation.cpp
 *
 *  Created on: May 2, 2021
 *      Author:
 */

#include <sstream>
#include <string>
#include "Interpretation.h"

const char* const Interpretation::interp = "interpretation";
const char* const Interpretation::function = "function";
const std::string Interpretation:: closing = std::string("])]).");


void Interpretation::read_model(const std::string&, int** model)
{

}


void Interpretation::parse_row(const std::string& line, int* vec)
{
	std::stringstream row(line);
	std::string el;
	int pos = 0;
	while(std::getline(row, el, ','))
    {
		std::stringstream elst(el);
        elst >> vec[pos];
        ++pos;
    }
}
