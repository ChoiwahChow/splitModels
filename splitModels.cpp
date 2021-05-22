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

static size_t min_models_in_file = 1;   	// minimum number of models in a file to be processed by Mace4's isofilter
static size_t model_size = 3000000;   		// initial space allocated for this number of models
static size_t max_num_functions = 5;  		// maximum number of binary functions in the model supported by this program

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
	std::string filter("isofilter");
	if (argc > 2) {
		filter = argv[2];
	}
	bool multiprocessing_on = false;
	bool find_biggest_only = false;
	if (argc > 3) {
		if (std::string(argv[3]) == "-t")
			multiprocessing_on = true;
		else if (std::string(argv[3]) == "-b")
			find_biggest_only = true;
	}
	int domain_size = atoi(argv[1]);

	/* set up buffer spaces for multiplication tables in a std::vector and invariant vectors in std::vector.
	 * Allow for max_num_functions for each of multiplication tables and invariant vectors.
	 */
	int mt_buf[domain_size*max_num_functions][domain_size];
	int* mt[domain_size*max_num_functions];
	int inv_vec_buf[domain_size*max_num_functions][Invariant::invariant_size];
	int* inv_vec[domain_size*max_num_functions];
	std::vector<int**> all_mt;
	std::vector<int**> all_inv_vec;

	for (size_t idx = 0; idx < max_num_functions; ++idx) {
		for (int jdx = 0; jdx < domain_size; ++jdx) {
			inv_vec[idx*domain_size+jdx] = reinterpret_cast<int*>(&inv_vec_buf[idx*domain_size+jdx]);
			mt[idx*domain_size+jdx] = reinterpret_cast<int*>(&mt_buf[idx*domain_size+jdx]);
		}
		all_mt.push_back(&mt[idx*domain_size]);
		all_inv_vec.push_back(&inv_vec[idx*domain_size]);
	}

	std::string line;
	std::vector<std::vector<std::string>> interps;  // stores all interpretations
	interps.reserve(model_size);
	std::stringstream ss;
	int next_key = 0;
	std::string key;
	std::unordered_map<std::string, int> buckets;

	while (std::cin) {
		int num_binop = Interpretation::parse_interpretation(std::cin, domain_size, all_mt, ss);
		if (num_binop <= 0)
			continue;
		Invariant::calc_invariant_vec(domain_size, num_binop, all_mt, all_inv_vec);
		Invariant::sort_invariant_vec(domain_size, num_binop, all_inv_vec);
		Invariant::hash_key(domain_size, num_binop, all_inv_vec, key);
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
	double duration = get_wall_time() - start_time;
	std::cerr << "Number of distinct invariant vector keys: " << next_key << " in " << duration << " seconds." << std::endl;
	//return EXIT_SUCCESS;

	size_t start_pos = 0;
	size_t end_pos = interps.size();
	if (find_biggest_only) {
		size_t biggest = interps[start_pos].size();
		for (size_t idx=0; idx < interps.size(); ++idx) {
			if (biggest < interps[idx].size()) {
				biggest = interps[idx].size();
				start_pos = idx;
			}
		}
		end_pos = start_pos + 1;
		std::cerr << "Biggest partition: " << biggest << std::endl;
	}

	double max_time = 0.0;
	size_t counter = 0;
	int num_file = 1;
	std::ofstream ofs;
	std::string filename("outputs/models" + std::to_string(num_file) + ".out");
	ofs.open(filename, std::ofstream::out);
	for (size_t idx = start_pos; idx < end_pos; ++idx) {
	    double start_time = get_wall_time();
		counter += interps[idx].size();
		for (size_t jdx=0; jdx < interps[idx].size(); ++jdx)
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
	std::cerr << "Number of distinct invariant vector keys: " << next_key << std::endl;
	return EXIT_SUCCESS;
}
