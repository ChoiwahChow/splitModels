/*
 * ArgumentParser.h
 *
 *  Created on: Jul 4, 2021
 *      Author: uabcoimbra
 */

#ifndef ARGUMENTPARSER_H_
#define ARGUMENTPARSER_H_

#include <string>

class ArgumentParser {
public:
	static const char arg_domain_size = 'd';
	static const char arg_min_models_in_file = 'm';
	static const char arg_sampling_frequency = 's';
	static const char arg_max_sample_size = 'x';
	static const char arg_num_random = 'r';
	static const char arg_multiprocessing_on = 'p';
	static const char arg_filter = 'f';
	static const char arg_find_biggest_only = 'b';
	static const char arg_output_file_prefix = 'o';
	static const char arg_statistics_file = 't';
	static const char arg_initial_random_seed = 'e';
public:
	int           domain_size;
	int           min_models_in_file;
	int           sampling_frequency;
	int           max_sample_size;
	int           num_random;
	bool          multiprocessing_on;
	std::string   mace_filter;
	bool          find_biggest_only;
	std::string   output_file_prefix;
	std::string   statistics_file;
	int           seed;

public:
	ArgumentParser(int argc, char* argv[]);
	int find_int_arg(const char arg, bool& found) const;
	std::string find_string_arg(const char arg, bool& found) const;
	bool find_bool_arg(const char arg, bool& found) const;
	virtual ~ArgumentParser();
};

#endif /* ARGUMENTPARSER_H_ */
