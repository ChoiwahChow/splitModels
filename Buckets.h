/*
 * Buckets.h
 *
 *  Created on: Jul 14, 2021
 *      Author: Choiwah Chow
 */

#ifndef BUCKETS_H_
#define BUCKETS_H_

#include <string>
#include <vector>
#include <unordered_map>
#include "TreeNode.h"


class Buckets {
private:
	static void copy_last_invariant(int domain_size, int base_invariants_length, std::vector<std::vector<std::vector<int>>>& all_inv_vec,
			const std::vector<std::vector<std::vector<int>>>& all_random_invariants, int pos, int best_invariant);

	static void hash_model(std::unordered_map<std::string, int>& blocks, const std::string& model_str,
			int& next_key, std::vector<std::vector<int>>& all_inv_vec, std::vector<std::vector<std::string>>& interps);

	static int calc_all_invariants(std::string& in_file, int domain_size, int& num_models, int starting_seed, int num_random,
			int max_sample_size, int sampling_frequency, std::vector<Tree>& trees, std::vector<int>& op_type, std::vector<std::string>& op_sym,
			std::vector<std::vector<std::vector<int>>>& all_mt, std::vector<int>& all_bin_function_mt, std::vector<int>& all_bin_relation_mt,
			std::vector<std::string>& bin_function_op_sym, std::vector<std::string>& bin_relation_op_sym,
			std::vector<std::vector<std::vector<int>>>& all_random_invariants, std::vector<std::vector<std::vector<int>>>& all_inv_vec,
			bool no_basic_invariants = false);

	static int  score_invariants(int domain_size, int base_invariants_length, std::vector<std::vector<std::vector<int>>>& all_inv_vec,
			std::vector<std::vector<std::vector<int>>>& all_random_invariants, const std::vector<int>& random_list);

	static int  find_best_random_invariants(int max_level, int domain_size, std::vector<std::vector<std::vector<int>>>& all_inv_vec,
			std::vector<std::vector<std::vector<int>>>& all_random_invariants, std::vector<int>& random_list);

	static int calc_selected_invariants(std::string& is, int domain_size, int& num_models, const std::vector<int>& random_list,
			std::vector<Tree>& trees, std::vector<int>& op_type, std::vector<std::string>& op_sym,
			std::vector<std::vector<std::vector<int>>>& all_mt, std::vector<int>& all_bin_function_mt, std::vector<int>& all_bin_relation_mt,
			std::vector<std::vector<std::string>>& interps, bool no_basic_invariants = false);
public:
	Buckets();
	virtual ~Buckets();

	// generate all buckets without random invariants
	static int bucketing(std::string& in_file, int domain_size, int& num_ops, int num_input_models, std::vector<std::vector<std::string>>& interps);

	// generate all buckets with random invariants
	static std::vector<int> bucketing(std::string& in_file, int domain_size, int starting_seed, int num_random, int max_sample_size,
			int sampling_frequency, int max_level, std::vector<std::vector<std::string>>& interps, bool no_basic_invariants);
};

#endif /* BUCKETS_H_ */
