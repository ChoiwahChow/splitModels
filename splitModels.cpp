//============================================================================
// Name        : splitModels.cpp
// Author      : Choiwah Chow
// Version     :
// Copyright   : Your copyright notice
// Description : Filter out isomorphic models with invariants, including
//               random invariants
//============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <time.h>
#include <sys/time.h>

#include "ArgumentParser.h"
#include "IsoFilter.h"
#include "Buckets.h"
#include "Utils.h"


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

	std::vector<std::vector<std::string>> interps;  // stores all interpretations
	int num_blocks = 1;
	int num_ops = 0;
	int num_models = argParser.num_input_models;
	int acutal_num_randoms = -1;
	double inv_calc_time = 0.0;
	double max_time = 0.0;

	if (argParser.num_random == 0 && argParser.no_basic_invariants) {
		IsoFilter::run_filter(in_file, argParser.output_file_prefix, argParser.mace_filter);
	}
	else {
		double calc_invariant_start = Utils::get_wall_time();
		if (argParser.num_random > 0) {
			std::vector<int> res = Buckets::bucketing(in_file, domain_size, argParser.seed, argParser.num_random,
					argParser.max_sample_size, argParser.sampling_frequency, argParser.max_random_level,
					argParser.output_file_prefix, argParser.mace_filter, interps, argParser.no_basic_invariants);
			num_ops = res[0];
			num_models = res[1];
			acutal_num_randoms = res[2];
			// int acutal_sample_size = res[3];
			// return EXIT_SUCCESS;
		}
		else {
			num_models = Buckets::bucketing(in_file, domain_size, num_ops, argParser.max_random_level,
					argParser.output_file_prefix, argParser.mace_filter, interps);
		}
		num_blocks = interps.size();
		std::cerr << "number of blocks: " << num_blocks << std::endl;
		inv_calc_time = Utils::get_wall_time() - calc_invariant_start;

		max_time = IsoFilter::run_filter(interps, argParser.output_file_prefix, argParser.mace_filter,
				argParser.min_models_in_file, argParser.find_biggest_only, argParser.num_threads);
	}

	double total_run_time = Utils::get_wall_time() - start_time;
	Utils::save_statistics(argParser.statistics_file, num_blocks, num_models, num_ops,
			argParser.sampling_frequency, acutal_num_randoms, inv_calc_time, total_run_time, max_time);

	return EXIT_SUCCESS;
}
