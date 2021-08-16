/*
 * IsoFilter.cpp
 *
 *  Created on: Jul 14, 2021
 *      Author: uabcoimbra
 */

#include <iostream>
#include <fstream>
#include <istream>
#include <sstream>
#include <time.h>
#include <sys/time.h>
#include "Utils.h"
#include "IsoFilter.h"

IsoFilter::IsoFilter() {
	// TODO Auto-generated constructor stub

}

IsoFilter::~IsoFilter() {
	// TODO Auto-generated destructor stub
}

double
IsoFilter::run_filter(const std::vector<std::vector<std::string>>& interps, const std::string& output_file_prefix,
		const std::string& mace_filter, unsigned int min_models_in_file, bool find_biggest_only, bool multiprocessing_on)
{
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
	std::string filename(output_file_prefix + std::to_string(num_file) + ".out");
	ofs.open(filename, std::ofstream::out);
	for (unsigned int idx = start_pos; idx < end_pos; ++idx) {
	    double start_time = Utils::get_wall_time();
		counter += interps[idx].size();
		for (unsigned int jdx=0; jdx < interps[idx].size(); ++jdx)
			ofs << interps[idx][jdx];
		if (counter >= min_models_in_file || idx == end_pos - 1) {
			counter = 0;
			ofs.close();
			std::string command("cat " + filename + " | " + mace_filter.c_str() + " >> " + filename + ".f ");
			if (multiprocessing_on && num_file % 5 != 0 && idx < interps.size() - 1)
				command += " &";
			int status = std::system(command.c_str());
			if (status != 0)
				std::cerr << "Error code in system call to spawn off process to filter out iso models from buckets." << std::endl;
			if (idx < interps.size() - 1) {
				if (multiprocessing_on)
					++num_file;
				filename = output_file_prefix + std::to_string(num_file) + ".out";
				ofs.open(filename, std::ofstream::out);
			}
		}
		double duration = Utils::get_wall_time() - start_time;
		if (duration > max_time)
			max_time = duration;
	}
	return max_time;
}

double
IsoFilter::run_filter(const std::string& in_file, const std::string& filename, std::string& mace_filter)
{
	std::string command("cat " + in_file + " | " + mace_filter.c_str() + " >> " + filename + "1.out.f ");
	int status = std::system(command.c_str());
	if (status != 0)
		std::cerr << "Error code in system call to spawn off process to filter out iso models from buckets." << std::endl;
	return 0;
}
