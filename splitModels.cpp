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

	int calc_invariant_start = Utils::get_wall_time();
	std::vector<std::vector<std::string>> interps;  // stores all interpretations
	int num_blocks = 1;
	int num_ops = 0;
	int num_models = 0;
	int acutal_num_randoms = -1;

	if (argParser.num_random > 0) {
		std::vector<int> res = Buckets::bucketing(in_file, domain_size, argParser.seed, argParser.num_random,
				argParser.max_sample_size, argParser.sampling_frequency, argParser.max_random_level, interps, argParser.no_basic_invariants);
		num_ops = res[0];
		num_models = res[1];
		acutal_num_randoms = res[2];
		int acutal_sample_size = res[3];

		std::cerr << "************Actual Sample Size: " << acutal_sample_size << "\n";
		std::cerr << "************Actual number of random invariant: " << acutal_num_randoms << std::endl;
		// return EXIT_SUCCESS;
	}
	else {
		Buckets::bucketing(in_file, domain_size, num_ops, argParser.max_random_level, interps);
	}
	num_blocks = interps.size();
	double inv_calc_time = Utils::get_wall_time() - calc_invariant_start;

	std::cerr << "************Full number of models processed: " << num_models << "\n";
	std::cerr << "************Number of blocks: " << num_blocks << "\n";
	std::cerr << "************Time for calculating invariants and blocks: " << inv_calc_time << "\n" << std::endl;

	double max_time = IsoFilter::run_filter(interps, argParser.output_file_prefix,
			argParser.mace_filter, argParser.min_models_in_file, argParser.find_biggest_only, argParser.multiprocessing_on);

	double total_run_time = Utils::get_wall_time() - start_time;
	Utils::save_statistics(argParser.statistics_file, num_blocks, num_models, num_ops,
			argParser.sampling_frequency, acutal_num_randoms, inv_calc_time, total_run_time, max_time);

	return EXIT_SUCCESS;
}
