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
const std::string Interpretation::ternary_op = std::string("(_,_,_)");
const std::string Interpretation::unary_op = std::string("(_)");
const std::string Interpretation::closing = std::string("])]).");
const std::string Interpretation::nested_closing = std::string("]),");


/*
 * Special case: ternary operation.
 * We only count the number of times a domain element appears in the table,
 * and do not construct the 3-d multiplication table.  That is, we actually
 * generate an invariant for it.
 */
void Interpretation::count_appearances(const std::string& line, std::vector<int>& vec)
{
	std::stringstream row(line);
	std::string el;
	int val = 0;
	while(std::getline(row, el, ','))
    {
		std::stringstream elst(el);
        elst >> val;
        vec[val] += 1;
    }
}


void Interpretation::parse_row(const std::string& line, std::vector<int>& vec)
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


std::vector<std::vector<int>> Interpretation::construct_1row_mt(int domain_size)
{
	std::vector<std::vector<int>> mt(1);
	for (int col = 0; col < domain_size; col++) {
		mt[0].push_back(0);
	}
	return mt;
}


std::vector<std::vector<int>> Interpretation::construct_2d_mt(int domain_size)
{
	std::vector<std::vector<int>> mt(domain_size);
	for (int idx = 0; idx < domain_size; idx++) {
		for (int col = 0; col < domain_size; col++) {
			mt[idx].push_back(0);
		}
	}
	return mt;
}

// TODO: allow specifying binop to process
int Interpretation::parse_interpretation(std::istream& is, int domain_size, std::vector<std::vector<std::vector<int>>>& all_mt,
		std::stringstream& ss, std::vector<int>& op_type, std::vector<std::string>& op_sym, bool extract)
{
	std::string line;
	bool in_binop = false;
	int  row = 0;
	bool done = false;
	int  op_pos = -1;
	bool extract_sym = op_sym.size() == 0;
	while (is && !done) {
		std::getline(is, line);
		if (extract) {
			ss << line << std::endl;
			if (in_binop) {
				parse_row(line, all_mt[op_pos][row]);
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
					if (extract_sym) {
						unsigned int paran_pos = line.find("(");
						op_sym.push_back(line.substr(paran_pos+1, binop_pos - paran_pos - 1));
						if (line.find(function, 0) != std::string::npos)
							op_type.push_back(InterpretationType::function);
						else {
							op_type.push_back(InterpretationType::relation);
						}
						all_mt.push_back(construct_2d_mt(domain_size));
					}
					++op_pos;
				}
				else {
					size_t unop_pos = line.find(unary_op, 0);
					if (unop_pos != std::string::npos) {
						if (extract_sym) {
							unsigned int paran_pos = line.find("(");
							op_sym.push_back(line.substr(paran_pos+1, unop_pos - paran_pos - 1));
							op_type.push_back(InterpretationType::unary_function);
							all_mt.push_back(construct_1row_mt(domain_size));
						}
						unsigned int left_paran_pos = line.find("[");
						std::string map = line.substr(left_paran_pos+1);
						++op_pos;
						parse_row(map, all_mt[op_pos][0]);
					}
					else {
						size_t ternop_pos = line.find(ternary_op, 0);
						if (ternop_pos != std::string::npos) {
							if (extract_sym) {
								unsigned int paran_pos = line.find("(");
								op_sym.push_back(line.substr(paran_pos+1, ternop_pos - paran_pos - 1));
								op_type.push_back(InterpretationType::ternary_function);
								all_mt.push_back(construct_1row_mt(domain_size));
							}
							unsigned int left_paran_pos = line.find("[");
							std::string map = line.substr(left_paran_pos+1);
							++op_pos;
							count_appearances(map, all_mt[op_pos][0]);
						}
					}
				}
			}
			if (!in_binop && line.find(interp, 0) != std::string::npos)
				continue;
		}

		if (!in_binop && line.find(closing) != std::string::npos)
			done = true;
	}
	return op_pos+1;
}
