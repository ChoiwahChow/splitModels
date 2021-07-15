/*
 * ArgumentParser.cpp
 *
 *  Created on: Jul 4, 2021
 *      Author: uabcoimbra
 */

#include <iostream>
#include "ArgumentParser.h"


ArgumentParser::ArgumentParser(int argc, char* argv[]):
	domain_size(-1), min_models_in_file(1), sampling_frequency(1), max_sample_size(1000000), num_random(0), multiprocessing_on(false),
	mace_filter("isofilter"), find_biggest_only(false), output_file_prefix("working/models"), statistics_file(""), seed(9)
{
	for(int idx=1; idx < argc; idx++) {
		char opt = argv[idx][0];
		if (opt != '-') {
			std::cerr << "Error: Invalid option: " << argv[idx] << ".  Option must start with \"-\"." << std::endl;
		}
		opt = argv[idx][1];
		switch (opt) {
		case arg_domain_size:
			domain_size = atoi(&argv[idx][2]);
			break;
		case arg_min_models_in_file:
			min_models_in_file = atoi(&argv[idx][2]);
			break;
		case arg_sampling_frequency:
			sampling_frequency = atoi(&argv[idx][2]);
			break;
		case arg_max_sample_size:
			max_sample_size = atoi(&argv[idx][2]);
			break;
		case arg_num_random:
			num_random = atoi(&argv[idx][2]);
			break;
		case arg_multiprocessing_on:
			multiprocessing_on = true;
			break;
		case arg_filter:
			mace_filter = std::string(argv[idx]).substr(2);
			break;
		case arg_find_biggest_only:
			find_biggest_only = true;
			break;
		case arg_output_file_prefix:
			output_file_prefix = std::string(argv[idx]).substr(2);
			break;
		case arg_statistics_file:
			statistics_file = std::string(argv[idx]).substr(2);
			break;
		case arg_initial_random_seed:
			seed = atoi(&argv[idx][2]);
			break;
		default:
			std::cerr << "Error: Invalid option " << argv[idx] << " ignored." << std::endl;
		}
	}
}

ArgumentParser::~ArgumentParser() {
	// TODO Auto-generated destructor stub
}

bool
ArgumentParser::find_bool_arg(char arg, bool& found) const
{

	found = true;
	if (arg == arg_multiprocessing_on)
		return multiprocessing_on;
	else if (arg == arg_find_biggest_only)
		return find_biggest_only;
	found = false;
	return false;
}

int
ArgumentParser::find_int_arg(char arg, bool& found) const
{
	found = true;
	switch (arg) {
	case arg_domain_size:
		if (domain_size == -1)
			found = false;
		return domain_size;
	case arg_min_models_in_file:
		return min_models_in_file;
	case arg_sampling_frequency:
		return sampling_frequency;
	case arg_max_sample_size:
		return max_sample_size;
	case arg_num_random:
		return num_random;
	case arg_initial_random_seed:
		return seed;
	}
	found = false;
	return -1;
}

std::string
ArgumentParser::find_string_arg(char arg, bool& found) const
{
	found = true;
	switch (arg) {
	case arg_filter:
		return mace_filter;
	case arg_output_file_prefix:
		return output_file_prefix;
	case arg_statistics_file:
		return statistics_file;
	}
	found = false;
	return "";
}
