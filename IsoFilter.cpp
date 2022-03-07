/*
 * IsoFilter.cpp
 *
 *  Created on: Jul 14, 2021
 *      Author: uabcoimbra
 */
#include <stdio.h>
#include <thread>
#include <cstdlib>
#include <dirent.h>
#include <iostream>
#include <fstream>
#include <istream>
#include <sstream>
#include <time.h>
#include <sys/time.h>
#include <array>
#include "Utils.h"
#include "IsoFilter.h"

IsoFilter::IsoFilter() {
	// TODO Auto-generated constructor stub

}

IsoFilter::~IsoFilter() {
	// TODO Auto-generated destructor stub
}

int
IsoFilter::run_filter(std::vector<std::string>& interps, const std::string& output_file_prefix, const std::string& mace_filter)
{
    char buffer[1024*1024];
	std::ofstream ofs;
	std::string filename(output_file_prefix + "_temp.out");
	ofs.open(filename, std::ofstream::out);
	for (unsigned int jdx=0; jdx < interps.size(); ++jdx)
		ofs << interps[jdx];
	ofs.close();
	std::string command("cat " + filename + " | " + mace_filter.c_str());
	FILE* pipe = popen(command.c_str(), "r");
	std::string models;
	while (fgets(buffer, sizeof buffer, pipe) != NULL) {
		models += buffer;
	}
	pclose(pipe);
	interps.clear();

	size_t pos = models.find("interpretation");  // there is at least one model
	bool done = false;
	while (!done) {
		size_t new_pos = models.find("interpretation", pos+1);
		if (new_pos == std::string::npos) {
			new_pos = models.find("% ", pos+1);
			done = true;
		}
		interps.push_back(models.substr(pos, new_pos));
		pos = new_pos;
	}
	return (int) interps.size();
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
			if (multiprocessing_on && num_file % 20 != 0 && idx < interps.size() - 1) {
				std::string p_file("parallel/" + std::to_string(num_file));
				command = "(touch " + p_file + "; " + command + "; rm " + p_file + ") &";
			}
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
	if (multiprocessing_on) {
		std::chrono::milliseconds timespan(1000);
		bool done = false;
		while (!done) {
			std::this_thread::sleep_for(timespan);
			done = true;
	        dirent* d;
	        DIR* dir = opendir("parallel");
	        readdir(dir);
	        readdir(dir);
	        if ((d = readdir(dir))!=NULL) {
	        	done = false;
	        }
	        closedir(dir);
		}
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
