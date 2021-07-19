/*
 * Buckets.h
 *
 *  Created on: Jul 14, 2021
 *      Author: uabcoimbra
 */

#ifndef BUCKETS_H_
#define BUCKETS_H_

#include <string>
#include <vector>
#include "InvariantsStore.h"


class Buckets {
public:
	static const unsigned int max_num_functions = 8;

	Buckets();
	virtual ~Buckets();

	static int bucketing(std::string& in_file, int domain_size, std::vector<int**>& all_mt, int* combo_inv_vec[],
			std::vector<int**>& all_inv_vec, std::vector<std::vector<std::string>>& interps);

	static int calc_all_invariants(std::string& is, int domain_size, int& num_models, int starting_seed, int& num_random,
			int max_sample_size, int sampling_frequency, std::vector<int*>& random_invariants, std::vector<Tree>& trees,
			std::vector<int>& op_type, std::vector<std::string>& op_sym, std::vector<int**>& all_inv_vec,
			std::vector<int**>& all_mt, std::vector<int**>& all_bin_function_mt, std::vector<int**>& all_bin_relation_mt,
			std::vector<std::string>& bin_function_op_sym, std::vector<std::string>& bin_relation_op_sym,
			std::vector<std::string>& models, InvariantsStore& inv_store);

	static int  build_buckets(int domain_size, int num_models, int num_ops, int* combo_inv_vec[], const std::vector<int>& random_list,
			std::vector<std::string>& models, InvariantsStore& inv_store, std::vector<std::vector<std::string>>& interps);

	static int  score_invariants(int domain_size, int num_models, int num_ops, int* combo_inv_vec[], const std::vector<int>& random_list,
			InvariantsStore& inv_store);

	static int  find_best_random_invariants(int max_level, int domain_size, int num_models, int num_ops, int* combo_inv_vec[], int num_random,
			std::vector<int>& random_list, InvariantsStore& inv_store);

	static int no_savings(std::string& is, int domain_size, int& num_models, int starting_seed, int& num_random,
			int max_sample_size, int sampling_frequency, std::vector<int*>& random_invariants, std::vector<Tree>& trees,
			std::vector<int>& op_type, std::vector<std::string>& op_sym, std::vector<int**>& all_inv_vec, int* combo_inv_vec[],
			std::vector<int**>& all_mt, std::vector<int**>& all_bin_function_mt, std::vector<int**>& all_bin_relation_mt,
			std::vector<std::string>& bin_function_op_sym, std::vector<std::string>& bin_relation_op_sym, std::vector<std::vector<std::string>>& interps);

	static int calc_selected_invariants(std::string& is, int domain_size, int& num_models, std::vector<int>& random_list,
			std::vector<int*>& random_invariants, std::vector<Tree>& trees,
			std::vector<int>& op_type, std::vector<std::string>& op_sym, std::vector<int**>& all_inv_vec,
			std::vector<int**>& all_mt, std::vector<int**>& all_bin_function_mt, std::vector<int**>& all_bin_relation_mt,
			std::vector<std::string>& models, InvariantsStore& inv_store);
};

#endif /* BUCKETS_H_ */
