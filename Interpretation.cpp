/*
 * Interpretation.cpp
 *
 *  Created on: May 2, 2021
 *      Author:
 */

#include <ostream>
#include <iostream>
#include "Interpretation.h"

const std::string Interpretation::interp = std::string("interpretation");
const std::string Interpretation::function = std::string("function");
const std::string Interpretation::relation = std::string("relation");
const std::string Interpretation::binary_op = std::string("(_,_)");
const std::string Interpretation::tenary_op = std::string("(_,_,_)");
const std::string Interpretation::unary_op = std::string("(_)");
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
int Interpretation::parse_interpretation(std::istream& is, int domain_size, std::vector<int**>& all_mt,
		std::stringstream& ss, std::vector<int>& op_type, std::vector<std::string>& op_sym, bool extract)
{
	std::string line;
	bool in_binop = false;
	int  row = 0;
	bool done = false;
	int  num_ops = 0;
	int**  mt;
	bool extract_sym = op_sym.size() == 0;
	while (is && !done) {
		std::getline(is, line);
		if (extract) {
			ss << line << std::endl;
			if (in_binop) {
				parse_row(line, mt[row]);
				++row;
				if (row >= domain_size) {
					in_binop = false;
				}
			}
			else if (line.find(function, 0) != std::string::npos || line.find(relation, 0) != std::string::npos) {
				size_t binop_pos = line.find(binary_op);
				if(binop_pos != std::string::npos) {
					in_binop = true;
					row = 0;
					mt = all_mt[num_ops];
					if (extract_sym) {
						unsigned int paran_pos = line.find("(");
						op_sym.push_back(line.substr(paran_pos+1, binop_pos - paran_pos - 1));
						if (line.find(function, 0) != std::string::npos)
							op_type[num_ops] = InterpretationType::function;
						else {
							op_type[num_ops] = InterpretationType::relation;
						}
					}
					++num_ops;
				}
				else {
					size_t unop_pos = line.find(unary_op, 0);
					if (unop_pos != std::string::npos) {
						mt = all_mt[num_ops];
						if (extract_sym) {
							unsigned int paran_pos = line.find("(");
							op_sym.push_back(line.substr(paran_pos+1, unop_pos - paran_pos - 1));
							op_type[num_ops] = InterpretationType::unary_function;
						}
						unsigned int left_paran_pos = line.find("[");
						std::string map = line.substr(left_paran_pos+1);
						parse_row(map, mt[0]);
						++num_ops;
					}
				}
			}
			if (!in_binop && line.find(interp, 0) != std::string::npos)
				continue;
		}

		if (!in_binop && line.find(closing) != std::string::npos)
			done = true;
	}
	return num_ops;
}
