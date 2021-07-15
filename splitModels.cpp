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

static unsigned int model_size = 3000000;   		// initial space allocated for this number of models


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
	int num_random = argParser.num_random;

	/* set up buffer spaces for multiplication tables in a std::vector and invariant vectors in std::vector.
	 * Allow for max_num_functions for each of multiplication tables and invariant vectors.
	 */
	int num_multiple = int((argParser.num_random+1)/10);

	int  mt_buf[domain_size*Buckets::max_num_functions][domain_size];
	int* mt[domain_size*Buckets::max_num_functions];
	int  inv_vec_buf[domain_size][Invariant::invariant_size*Buckets::max_num_functions];
	int* inv_vec[domain_size*Buckets::max_num_functions];  // each binary function occupies domain_size contiguous slots
	std::vector<int**> all_mt;      // each binary function occupies one slot in this vector
	std::vector<int**> all_inv_vec; // each binary function occupies one slot in this vector
	int* combo_inv_vec[domain_size];

	for (unsigned int idx = 0; idx < Buckets::max_num_functions; ++idx) {
		for (int jdx = 0; jdx < domain_size; ++jdx) {
			inv_vec[idx*domain_size+jdx] = &inv_vec_buf[jdx][idx*Invariant::invariant_size];
			mt[idx*domain_size+jdx] = reinterpret_cast<int*>(&mt_buf[idx*domain_size+jdx]);
		}
		all_mt.push_back(&mt[idx*domain_size]);
		all_inv_vec.push_back(&inv_vec[idx*domain_size]);  // each element of all_inv_vec points to an array of size domain elements
	}
	for (int idx = 0; idx < domain_size; ++idx)
		combo_inv_vec[idx] = inv_vec[idx];

	std::string line;
	std::vector<std::vector<std::string>> interps;  // stores all interpretations
	interps.reserve(model_size);
	std::stringstream ss;
	int next_key = 0;
	std::string key;
	std::unordered_map<std::string, int> buckets;
	std::vector<int> op_type(Buckets::max_num_functions);
	std::vector<std::string> op_sym;

	std::vector<Tree> trees(argParser.num_random);
	std::vector<int*> precalced(argParser.num_random); // each points to an array of size domain_size, one invariant for each domain element in this array
	std::vector<int**> all_bin_function_mt;
	std::vector<std::string> bin_function_op_sym;
	std::vector<int**> all_bin_relation_mt;
	std::vector<std::string> bin_relation_op_sym;

	RandomInvariants  ri_gen = RandomInvariants();
	InvariantsStore inv_store(domain_size, argParser.num_random, argParser.max_sample_size);
	int num_models = -1;
	int num_models_processed = 0;
	while (std::cin) {
		num_models++;
		bool to_process = (num_models_processed < argParser.max_sample_size) && (num_models % argParser.sampling_frequency == 0);
		int num_ops = Interpretation::parse_interpretation(std::cin, domain_size, all_mt, ss,
				op_type, op_sym, to_process);
		if (num_ops <= 0)
			continue;
		ri_gen.init_random_invariants(domain_size, argParser.seed, num_random, num_multiple,
					trees, op_type, op_sym, all_mt, all_bin_function_mt, all_bin_relation_mt,
					bin_function_op_sym, bin_relation_op_sym, precalced);
		ri_gen.calc_random_invariants(domain_size, num_random, trees, all_bin_function_mt, precalced);

		std::string tree_hash_key;
		Invariant::calc_invariant_vec(domain_size, num_ops, all_mt, all_inv_vec, op_type, op_sym, precalced);
		Invariant::sort_invariant_vec(domain_size, num_ops+num_multiple, combo_inv_vec);
		Invariant::hash_key(domain_size, num_ops+num_multiple, combo_inv_vec, key);

		inv_store.save_invariants(all_inv_vec, precalced);

		int compact_key;
		if (buckets.find(key) != buckets.end()) {
			compact_key = buckets[key];
		}
		else {
			compact_key = next_key;
			next_key++;
			buckets[key] = compact_key;
			interps.push_back(std::vector<std::string>());
		}
		interps[compact_key].push_back(ss.str());
		ss.str("");
		num_models_processed++;
	}
	double inv_calc_time = Utils::get_wall_time() - start_time;
	std::cerr << "Number of models " << num_models << " Processed " << num_models_processed << std::endl;
	std::cerr << "Sampling frequency: " << argParser.sampling_frequency << std::endl;
	std::cerr << "Number of random invariants: " << num_random << ".   Starting random seed: " << argParser.seed << std::endl;
	std::cerr << "Number of buckets: " << next_key << " in " << inv_calc_time << " seconds." << std::endl;
	//return EXIT_SUCCESS;

	double max_time = IsoFilter::run_filter(interps, argParser.output_file_prefix,
			argParser.mace_filter, argParser.min_models_in_file, argParser.find_biggest_only, argParser.multiprocessing_on);

	double total_run_time = Utils::get_wall_time() - start_time;
	Utils::save_statistics(argParser.statistics_file, next_key, num_models, argParser.sampling_frequency, num_random, inv_calc_time, total_run_time, max_time);

	return EXIT_SUCCESS;
}
