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

#include "Invariant.h"
#include "Interpretation.h"
#include "TreeNode.h"

static unsigned int default_min_models_in_file = 1;   	// minimum number of models in a file to be processed by Mace4's isofilter
static unsigned int model_size = 3000000;   		// initial space allocated for this number of models
static unsigned int max_num_functions = 106;  		// maximum number of binary functions in the model supported by this program

double get_wall_time(){
    struct timeval time;
    if (gettimeofday(&time,NULL)){
        //  Handle error
        return 0;
    }
    return (double)time.tv_sec + (double)time.tv_usec * .000001;
}

int main(int argc, char* argv[])
{
    double start_time = get_wall_time();
	if (argc < 2) {
		std::cerr << "Missing argument: domain size." << std::endl;
		return -1;
	}
	unsigned int min_models_in_file = default_min_models_in_file;
	if (argc > 2) {
		min_models_in_file = atoi(argv[2]);
	}
	std::string filter("isofilter");
	if (argc > 3) {
		filter = argv[3];
	}
	bool multiprocessing_on = false;
	bool find_biggest_only = false;
	if (argc > 4) {
		if (std::string(argv[4]) == "-t")
			multiprocessing_on = true;
		else if (std::string(argv[4]) == "-b")
			find_biggest_only = true;
	}
	int domain_size = atoi(argv[1]);

	/* set up buffer spaces for multiplication tables in a std::vector and invariant vectors in std::vector.
	 * Allow for max_num_functions for each of multiplication tables and invariant vectors.
	 */
	int num_random = Invariant::invariant_size*100;

	int  mt_buf[domain_size*max_num_functions][domain_size];
	int* mt[domain_size*max_num_functions];
	int  inv_vec_buf[domain_size][Invariant::invariant_size*max_num_functions];
	int* inv_vec[domain_size*max_num_functions];  // each binary function occupies domain_size contiguous slots
	std::vector<int**> all_mt;      // each binary function occupies one slot in this vector
	std::vector<int**> all_inv_vec; // each binary function occupies one slot in this vector
	int* combo_inv_vec[domain_size];

	for (unsigned int idx = 0; idx < max_num_functions; ++idx) {
		for (int jdx = 0; jdx < domain_size; ++jdx) {
			inv_vec[idx*domain_size+jdx] = &inv_vec_buf[jdx][idx*Invariant::invariant_size];
			mt[idx*domain_size+jdx] = reinterpret_cast<int*>(&mt_buf[idx*domain_size+jdx]);
		}
		all_mt.push_back(&mt[idx*domain_size]);
		all_inv_vec.push_back(&inv_vec[idx*domain_size]);
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

	int seed = 9;
	std::vector<Tree> trees(num_random);
	for( int idx=0; idx < num_random; idx++) {
		trees[idx] = Tree(all_mt[0], 4, 3);
		trees[idx].initialize(seed++);
	}
	std::vector<int*> precalced(num_random);
	while (std::cin) {
		int num_binop = Interpretation::parse_interpretation(std::cin, domain_size, all_mt, ss);
		if (num_binop <= 0)
			continue;
		for( int idx=0; idx < num_random; idx++) {
			trees[idx].calc_invariant_vector(domain_size);
			precalced[idx] = trees[idx].get_invariants();
		}
		std::string tree_hash_key;
		/*
		Invariant::calc_cross_invariant_vec(domain_size, num_binop, all_mt, all_inv_vec);
		Invariant::sort_cross_invariant_vec(domain_size, combo_inv_vec);
		Invariant::cross_hash_key(domain_size, combo_inv_vec, key);
		*/
		Invariant::calc_invariant_vec(domain_size, num_binop, all_mt, all_inv_vec, precalced);
		Invariant::sort_invariant_vec(domain_size, num_binop+10, combo_inv_vec);
		Invariant::hash_key(domain_size, num_binop+10, combo_inv_vec, key);

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
	}
	double inv_calc_time = get_wall_time() - start_time;
	std::cerr << "Number of distinct invariant vector keys: " << next_key << " in " << inv_calc_time << " seconds." << std::endl;
	return EXIT_SUCCESS;

	unsigned int start_pos = 0;
	unsigned int end_pos = interps.size();
	if (find_biggest_only) {
		unsigned int biggest = interps[start_pos].size();
		for (unsigned int idx=0; idx < interps.size(); ++idx) {
			if (biggest < interps[idx].size()) {
				biggest = interps[idx].size();
				start_pos = idx;
			}
		}
		end_pos = start_pos + 1;
		std::cerr << "Biggest partition: " << biggest << std::endl;
	}

	double max_time = 0.0;
	unsigned int counter = 0;
	int num_file = 1;
	std::ofstream ofs;
	std::string filename("outputs/models" + std::to_string(num_file) + ".out");
	ofs.open(filename, std::ofstream::out);
	for (unsigned int idx = start_pos; idx < end_pos; ++idx) {
	    double start_time = get_wall_time();
		counter += interps[idx].size();
		for (unsigned int jdx=0; jdx < interps[idx].size(); ++jdx)
			ofs << interps[idx][jdx];
		if (counter >= min_models_in_file || idx == end_pos - 1) {
			counter = 0;
			ofs.close();
			std::string command("cat " + filename + " | " + filter + " >> " + filename + ".f ");
			if (num_file % 5 != 0 && idx < interps.size() - 1 && multiprocessing_on)
				command += " &";
			std::system(command.c_str());
			if (idx < interps.size() - 1) {
				if (multiprocessing_on)
					++num_file;
				filename = "outputs/models" + std::to_string(num_file) + ".out";
				ofs.open(filename, std::ofstream::out);
			}
		}
		double duration = get_wall_time() - start_time;
		if (duration > max_time)
			max_time = duration;
	}
	std::cerr << "Maximum Processing time " << max_time << std::endl;
	std::cerr << "Number of distinct invariant vector keys: " << next_key << " in " << inv_calc_time << " seconds." << std::endl;
	std::cerr << "Total run time: " << get_wall_time() - start_time << std::endl;
	return EXIT_SUCCESS;
}
