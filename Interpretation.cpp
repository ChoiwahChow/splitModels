/*
 * Interpretation.cpp
 *
 *  Created on: May 2, 2021
 *      Author:
 */

#include <ostream>
#include "Interpretation.h"

const std::string Interpretation::interp = std::string("interpretation");
const std::string Interpretation::function = std::string("function");
const std::string Interpretation::binary_op = std::string("(_,_");
const std::string Interpretation::closing = std::string("])]).");
const std::string Interpretation::nested_closing = std::string("]),");


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

// TODO: allow specifying binop to process
int Interpretation::parse_interpretation(std::istream& is, int domain_size, std::vector<int**> all_mt, std::stringstream& ss)
{
	std::string line;
	bool in_binop = false;
	int  row = 0;
	bool done = false;
	int  num_binop = 0;
	int**  mt;
	while (is && !done) {
		std::getline(is, line);
		ss << line << std::endl;
		if (in_binop) {
			parse_row(line, mt[row]);
			++row;
			if (row >= domain_size) {
				in_binop = false;
			}
		}
		else if (line.find(function, 0) != std::string::npos) {
			if(line.find(binary_op) != std::string::npos) {
				in_binop = true;
				row = 0;
				mt = all_mt[num_binop];
				++num_binop;
			}
		}

		if (!in_binop && line.find(interp, 0) != std::string::npos)
			continue;
		if (!in_binop && line.find(closing) != std::string::npos)
			done = true;
	}
	return num_binop;
}
