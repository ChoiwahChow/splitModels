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
#include <algorithm>
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

int
IsoFilter::find_slot(int num_threads)
{
	std::vector<bool> slots(num_threads+1, false);
	int slot = 0;
	DIR* dir = opendir("parallel");
       	dirent* d;
	while ((d=readdir(dir)) != NULL) {
		// std::cerr << "dir******************" << d->d_name << std::endl;
		const char* fn = d->d_name;
		if (fn[0] <= '9' && fn[0] >= '0')
			slots[std::stoi(fn)] = true; 
	}
       	closedir(dir);
	slot = num_threads;
	while (slots[slot]) {
		slot--;
	}
	// std::cerr << "out**************" << slot << std::endl;
	return slot;
}

double
IsoFilter::run_filter(std::vector<std::vector<std::string>>& interps, const std::string& output_file_prefix,
		const std::string& mace_filter, unsigned int min_models_in_file, bool find_biggest_only, int num_threads)
{
	unsigned int end_pos = interps.size();
	std::sort(interps.begin(), interps.end(), [](const std::vector<std::string> & a, const std::vector<std::string> & b){ return a.size() > b.size(); });
	if (find_biggest_only)
		end_pos = 1;

	double max_time = 0.0;
	unsigned int counter = 0;
	std::chrono::milliseconds timespan(200);
	std::ofstream ofs;
	int start_idx = 0;
	double start_time = Utils::get_wall_time();
	int batch_count = 0;
	for (unsigned int idx = 0; idx < end_pos; ++idx) {
		// std::cerr << "**************start_idx " << start_idx << std::endl;
		counter += interps[idx].size();
		batch_count++;
		if (batch_count % 1000 == 0)
			std::cout << "Debug******************* doing batch " << batch_count << std::endl;
		if (counter < min_models_in_file && idx < end_pos - 1) {
			continue;
		}

		// std::cerr << "**************counter " << counter << "*****" << num_threads << std::endl;
		counter = 0;
		int slot = find_slot(num_threads);
		//std::cerr << "**************slot " << slot << std::endl;
		while (slot == 0) {
			std::this_thread::sleep_for(timespan);
			slot = find_slot(num_threads);
		}
		std::string filename(output_file_prefix + std::to_string(slot) + ".out");
		ofs.open(filename, std::ofstream::out);
		for (unsigned int pos = start_idx; pos <=idx; pos++) {
			for (unsigned int jdx=0; jdx < interps[pos].size(); ++jdx)
				ofs << interps[pos][jdx];
		}
		ofs.close();
		start_idx = idx + 1;
		std::string p_file("parallel/" + std::to_string(slot));
		std::ofstream ctl_file;
		ctl_file.open(p_file, std::ofstream::out);
		ctl_file.close();
		std::string command("(cat " + filename + " | " + mace_filter.c_str() + " >> " + filename + ".f; rm " + p_file + ") &");
		//std::cerr << command << std::endl;
		int status = std::system(command.c_str());
		if (status != 0)
			std::cerr << "Error code in system call to spawn off process to filter out iso models from buckets." << std::endl;
		double duration = Utils::get_wall_time() - start_time;
		if (duration > max_time)
			max_time = duration;
	}
	bool done = false;
	while (!done) {
		std::this_thread::sleep_for(timespan);
		done = true;
	        dirent* d;
	        DIR* dir = opendir("parallel");
		// get rid of . and ..
	        readdir(dir);
	        readdir(dir);
	        if ((d = readdir(dir))!=NULL) {
	        	done = false;
	        }
	        closedir(dir);
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
