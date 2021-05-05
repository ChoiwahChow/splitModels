//============================================================================
// Name        : splitModels.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C, Ansi-style
//============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <unordered_map>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#include "Invariant.h"
#include "Interpretation.h"

static size_t min_models_in_file = 100000;
static size_t model_size = 1000000;


int main(int argc, char* argv[])
{
	if (argc < 2) {
		std::cerr << "Missing argument: domain size." << std::endl;
		return -1;
	}
	bool multiprocessing_on = false;
	if (argc > 2) {
		if (std::string(argv[2]) == "t")
			multiprocessing_on = true;
	}
	int domain_size = atoi(argv[1]);
	int mt_buf[domain_size][domain_size];
	int* mt[domain_size];
	int inv_vec_buf[domain_size][Invariant::invariant_size];
	int* inv_vec[domain_size];
	for (int idx = 0; idx < domain_size; ++idx) {
		inv_vec[idx] = reinterpret_cast<int*>(&inv_vec_buf[idx]);
		mt[idx] = reinterpret_cast<int*>(&mt_buf[idx]);
	}

	std::string line;
	std::vector<std::vector<std::string>> interps;  // stores all interpretations
	interps.reserve(model_size);
	std::stringstream ss;
	int row = 0;
	int next_key = 0;
	std::string key;
	std::unordered_map<std::string, int> buckets;
	while (std::cin) {
		std::getline(std::cin, line);
		ss << line << std::endl;
		if (line.find(Interpretation::interp, 0) != std::string::npos || line.find(Interpretation::function, 0) != std::string::npos)
			continue;

		size_t endpos = line.find(Interpretation::closing);
		if (line.find(Interpretation::closing) != std::string::npos) {
			line.replace(endpos, Interpretation::closing.length(), ",");
			Interpretation::parse_row(line, mt[row]);
			row = 0;
			Invariant::calc_invariant_vec(domain_size, reinterpret_cast<int**>(mt), reinterpret_cast<int**>(inv_vec));
			Invariant::hash_key(domain_size, reinterpret_cast<int**>(inv_vec), key);
			int new_key;
			if (buckets.find(key) != buckets.end()) {
				new_key = buckets[key];
			}
			else {
				new_key = next_key++;
				buckets[key] = new_key;
				interps.push_back(std::vector<std::string>());
			}
			interps[new_key].push_back(ss.str());
			ss.str("");
		}
		else {
			Interpretation::parse_row(line, mt[row]);
			++row;
		}
	}
	size_t counter = 0;
	int num_file = 1;
	std::ofstream ofs;
	std::string filename("outputs/models" + std::to_string(num_file) + ".out");
	ofs.open(filename, std::ofstream::out);
	for (size_t idx=0; idx < interps.size(); ++idx) {
		counter += interps[idx].size();
		for (size_t jdx=0; jdx < interps[idx].size(); ++jdx)
			ofs << interps[idx][jdx];
		if (counter >= min_models_in_file || idx == interps.size() - 1) {
			counter = 0;
			ofs.close();
			std::string command("cat " + filename + " | isofilter >> " + filename + ".f ");
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
	}
	std::cerr << "Number of keys: " << next_key << std::endl;
	return EXIT_SUCCESS;
}
