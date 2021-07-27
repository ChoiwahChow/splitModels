/*
 * RandomInvariants.h
 *
 *  Created on: Jul 15, 2021
 *      Author: uabcoimbra
 */

#ifndef RANDOMINVARIANTS_H_
#define RANDOMINVARIANTS_H_

#include <string>
#include <vector>
#include "TreeNode.h"

class RandomInvariants {
private:
	bool tree_initialized;
public:
	RandomInvariants(): tree_initialized(false) {};
	virtual ~RandomInvariants();

	void init_random_invariants(int domain_size, int starting_seed, int& num_random,
			std::vector<Tree>& trees, std::vector<int>& op_type, std::vector<std::string>& op_sym,
			std::vector<std::vector<std::vector<int>>>& all_mt, std::vector<int>& all_bin_function_mt,
			std::vector<int>& all_bin_relation_mt, std::vector<std::string>& bin_function_op_sym,
			std::vector<std::string>& bin_relation_op_sym, std::vector<int*>& random_invariants);

	void calc_random_invariants(int domain_size, int& num_random, std::vector<Tree>& trees,
			std::vector<int>& all_bin_function_mt, std::vector<int*>& random_invariants);
	void calc_random_invariants(int domain_size, std::vector<int>& random_list, std::vector<Tree>& trees,
			std::vector<int>& all_bin_function_mt, std::vector<int*>& random_invariants);
};

#endif /* RANDOMINVARIANTS_H_ */
