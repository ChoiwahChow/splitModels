/*
 * RandomInvariants.cpp
 *
 *  Created on: Jul 15, 2021
 *      Author: uabcoimbra
 */

#include "Interpretation.h"
#include "Invariant.h"
#include "RandomInvariants.h"

RandomInvariants::~RandomInvariants() {
	// TODO Auto-generated destructor stub
}

void RandomInvariants::init_random_invariants(int domain_size, int starting_seed, int& num_random,
		std::vector<Tree>& trees, std::vector<int>& op_type, std::vector<std::string>& op_sym,
		std::vector<std::vector<std::vector<int>>>& all_mt, std::vector<int>& all_bin_function_mt,
		std::vector<int>& all_bin_relation_mt, std::vector<std::string>& bin_function_op_sym,
		std::vector<std::string>& bin_relation_op_sym, std::vector<std::vector<int>>& random_invariants)
{
	if (tree_initialized)
		return;
	tree_initialized = true;

	for( size_t idx=0; idx < op_sym.size(); idx++) {
		if (op_type[idx] == InterpretationType::function) {
			all_bin_function_mt.push_back(idx);
			bin_function_op_sym.push_back(op_sym[idx]);
		}
		else if (op_type[idx] == InterpretationType::relation) {
			all_bin_relation_mt.push_back(idx);
			bin_relation_op_sym.push_back(op_sym[idx]);
		}
	}
	int seed = starting_seed;
	if (!all_bin_function_mt.empty()) {
		for( int idx=0; idx < num_random; idx++) {
			trees[idx] = Tree(4, 2);
			trees[idx].initialize(all_mt, all_bin_function_mt, bin_function_op_sym,
					all_bin_relation_mt, bin_relation_op_sym, seed++);
		}
	}
	else {
		num_random = 0;
		random_invariants.clear();
	}
}


void RandomInvariants::calc_random_invariants(int domain_size, int& num_random, std::vector<Tree>& trees,
		std::vector<int>& all_bin_function_mt, std::vector<std::vector<int>>& random_invariants)
{
	if (!all_bin_function_mt.empty()) {
		for( int idx=0; idx < num_random; idx++) {
			trees[idx].calc_invariant_vector(domain_size);
			random_invariants[idx] = trees[idx].get_invariants();
		}
		/* debug print only
		std::cerr << "w**calc_random_invariants, random_invariants**\n";
		for( int idx=0; idx < num_random; idx++) {
			for (int jdx = 0; jdx < domain_size; jdx++)
				std::cerr << random_invariants[idx][jdx] << " ";
			std::cerr << "\n";
		}
		std::cerr << "w********" << std::endl;
		*/
	}
}


void RandomInvariants::calc_random_invariants(int domain_size, std::vector<int>& random_list, std::vector<Tree>& trees,
		std::vector<int>& all_bin_function_mt, std::vector<std::vector<int>>& random_invariants)
{
	if (!all_bin_function_mt.empty()) {
		for( size_t idx=0; idx < random_list.size(); idx++) {
			trees[random_list[idx]].calc_invariant_vector(domain_size);
			random_invariants[idx] = trees[random_list[idx]].get_invariants();
		}
	}
}

