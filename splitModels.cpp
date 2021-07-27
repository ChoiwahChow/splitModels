//============================================================================
// Name        : splitModels.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C, Ansi-style
//============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <unordered_map>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <time.h>
#include <sys/time.h>

#include "ArgumentParser.h"
#include "Invariant.h"
#include "Interpretation.h"
#include "TreeNode.h"
#include "InvariantsStore.h"
#include "IsoFilter.h"
#include "Buckets.h"
#include "Utils.h"
#include "RandomInvariants.h"


int main(int argc, char* argv[])
{
	// process all input arguments
    double start_time = Utils::get_wall_time();
    ArgumentParser argParser(argc, argv);
    int domain_size = argParser.domain_size;
	if (domain_size == -1) {
		std::cerr << "Missing argument: domain size." << std::endl;
		return -1;
	}
	std::string in_file = argParser.interpretation_file_name;
	if (in_file == "") {
		std::cerr << "Missing argument: Input interpretation file path." << std::endl;
		return -1;
	}

	int input_num_random = argParser.num_random;
	int num_random = input_num_random;   // could be re-set to zero if no random invariant is possible - e.g. no binary function

	/* set up buffer spaces for multiplication tables in a std::vector and invariant vectors in std::vector.
	 * Allow for max_num_functions for each of multiplication tables and invariant vectors.
	 */
	std::vector<std::vector<std::vector<int>>>  all_mt;

	int  inv_vec_buf[domain_size][Invariant::invariant_size*Buckets::max_num_functions+input_num_random];
	int* inv_vec[domain_size*Buckets::max_num_functions+1];  // each binary function occupies domain_size contiguous slots, last "1" for random invariants
	std::vector<int**> all_inv_vec; // each binary function occupies one slot in this vector
	int* combo_inv_vec[domain_size];

	for (unsigned int idx = 0; idx < Buckets::max_num_functions; ++idx) {
		for (int jdx = 0; jdx < domain_size; ++jdx) {
			inv_vec[idx*domain_size+jdx] = &inv_vec_buf[jdx][idx*Invariant::invariant_size];
		}
		all_inv_vec.push_back(&inv_vec[idx*domain_size]);  // each element of all_inv_vec points to an array of size domain elements
	}
	for (int idx = 0; idx < domain_size; ++idx)
		combo_inv_vec[idx] = inv_vec_buf[idx];

	/* Calculations using samples of interpretations and all random invariants to find the best subset of randome invariants
	 */
	std::vector<int> op_type;
	std::vector<std::string> op_sym;

	std::vector<Tree> trees(input_num_random);
	std::vector<int*> random_invariants(input_num_random); // each points to an array of size domain_size, one invariant for each domain element in this array
	std::vector<int> all_bin_function_mt;
	std::vector<std::string> bin_function_op_sym;
	std::vector<int> all_bin_relation_mt;
	std::vector<std::string> bin_relation_op_sym;
	std::vector<std::string> models;

	InvariantsStore inv_store(domain_size, Buckets::max_num_functions, input_num_random, argParser.max_sample_size);

	int num_models = 0;
	int num_models_processed = Buckets::calc_all_invariants(in_file, domain_size, num_models, argParser.seed, num_random,
			argParser.max_sample_size, argParser.sampling_frequency, random_invariants, trees, op_type, op_sym, all_inv_vec,
			all_mt, all_bin_function_mt, all_bin_relation_mt, bin_function_op_sym, bin_relation_op_sym, models, inv_store,
			argParser.no_basic_invariants);

	std::cerr << "************Number of (Sampling) models processed: " << num_models_processed
			<< " No basic invariants: " << argParser.no_basic_invariants << std::endl;
	int num_ops = op_sym.size();
	std::vector<int> random_list;

	int start_time_best_random = Utils::get_wall_time();
	Buckets::find_best_random_invariants(argParser.max_random_level, domain_size, num_models_processed, num_ops, combo_inv_vec, num_random, random_list, inv_store);
	std::cerr << "************Time to find best random invariants " << Utils::get_wall_time() - start_time_best_random << "\n" << std::endl;
	// return EXIT_SUCCESS;

	/* Calculations using full set of interpretations and the best subset of random invariants
	 */
	int calc_selected_random_start = Utils::get_wall_time();
	std::vector<std::vector<std::string>> interps;  // stores all interpretations
	interps.reserve(num_models);
	random_invariants.resize(random_list.size());

	num_models_processed = Buckets::calc_selected_invariants(in_file, domain_size, num_models, random_list,
			random_invariants, trees, op_type, op_sym, all_inv_vec, combo_inv_vec,
			all_mt, all_bin_function_mt, all_bin_relation_mt, models, interps, argParser.no_basic_invariants);

	double inv_calc_time = Utils::get_wall_time() - start_time;
	int    num_buckets = interps.size();
	std::cerr << "************Time to calculate invariants including selected random variants " << Utils::get_wall_time() - calc_selected_random_start << std::endl;
	std::cerr << "************Full number of models processed: " << num_models_processed << std::endl;
	std::cerr << "************Actual number of random invariant: " << random_list.size() << std::endl;
	std::cerr << "************Number of buckets: " << num_buckets << std::endl;
	std::cerr << "************Time for calculating invariants: " << inv_calc_time << "\n" << std::endl;
	// return EXIT_SUCCESS;

	double max_time = IsoFilter::run_filter(interps, argParser.output_file_prefix,
			argParser.mace_filter, argParser.min_models_in_file, argParser.find_biggest_only, argParser.multiprocessing_on);

	double total_run_time = Utils::get_wall_time() - start_time;
	Utils::save_statistics(argParser.statistics_file, num_buckets, num_models, num_ops,
			argParser.sampling_frequency, random_list.size(), inv_calc_time, total_run_time, max_time);

	return EXIT_SUCCESS;
}
