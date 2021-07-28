/*
 * Buckets.cpp
 *
 *  Created on: Jul 14, 2021
 *      Author: uabcoimbra
 */

#include <istream>
#include <fstream>
#include <unordered_map>
#include "Interpretation.h"
#include "Invariant.h"
#include "RandomInvariants.h"
#include "Buckets.h"

Buckets::Buckets() {
	// TODO Auto-generated constructor stub

}

Buckets::~Buckets() {
	// TODO Auto-generated destructor stub
}


/*
 * This is for computing all invariants: basic invariants and the all random invariants,
 * and save into InvariantsStore
 */
int Buckets::calc_all_invariants(std::string& in_file, int domain_size, int& num_models, int starting_seed, int num_random,
		int max_sample_size, int sampling_frequency, std::vector<Tree>& trees,
		std::vector<int>& op_type, std::vector<std::string>& op_sym, std::vector<int**>& all_inv_vec,
		std::vector<std::vector<std::vector<int>>>& all_mt, std::vector<int>& all_bin_function_mt, std::vector<int>& all_bin_relation_mt,
		std::vector<std::string>& bin_function_op_sym, std::vector<std::string>& bin_relation_op_sym,
		std::vector<std::string>& models, InvariantsStore& inv_store, bool no_basic_invariants)
{
	std::stringstream ss;
	int num_models_processed = 0;
	std::vector<std::vector<int>> random_invariants(num_random);
	RandomInvariants  ri_gen = RandomInvariants();
	num_models = -1;
	std::ifstream is;
	is.open(in_file);
	while (is) {
		num_models++;
		bool to_process = (num_models_processed < max_sample_size) && (num_models % sampling_frequency == 0);
		int num_ops = Interpretation::parse_interpretation(is, domain_size, all_mt, ss, op_type, op_sym, to_process);
		if (num_ops <= 0)
			continue;
		ri_gen.init_random_invariants(domain_size, starting_seed, num_random,
					trees, op_type, op_sym, all_mt, all_bin_function_mt, all_bin_relation_mt,
					bin_function_op_sym, bin_relation_op_sym, random_invariants);
		ri_gen.calc_random_invariants(domain_size, num_random, trees, all_bin_function_mt, random_invariants);

        Invariant::calc_invariant_vec(domain_size, num_ops, all_mt, all_inv_vec, op_type, op_sym, no_basic_invariants);
        inv_store.save_invariants(num_ops, all_inv_vec, random_invariants);
        models.push_back(ss.str());
		ss.str("");
        num_models_processed++;
        if( (size_t)num_ops != op_sym.size())
        	std::cerr << "Error: ******************************** num_ops != op_sym.size " << num_ops << std::endl;
	}
	is.close();
	return num_models_processed;
}


/*
 * This is for computing all invariants: basic invariants and the subset of best random invariants,
 * no need to save the invariants
 */
int Buckets::calc_selected_invariants(std::string& in_file, int domain_size, int& num_models, std::vector<int>& random_list,
		std::vector<Tree>& trees, std::vector<int>& op_type, std::vector<std::string>& op_sym, std::vector<int**>& all_inv_vec, int* combo_inv_vec[],
		std::vector<std::vector<std::vector<int>>>& all_mt, std::vector<int>& all_bin_function_mt, std::vector<int>& all_bin_relation_mt,
		std::vector<std::string>& models, std::vector<std::vector<std::string>>& interps, bool no_basic_invariants)
{
	std::stringstream ss;
	int num_models_processed = 0;
	std::vector<std::vector<int>> random_invariants(random_list.size());
	RandomInvariants  ri_gen = RandomInvariants();
	std::unordered_map<std::string, int> buckets;
	num_models = -1;
	std::string key;
	int next_key = 0;
	int num_random = random_list.size();
	std::ifstream is;
	is.open(in_file);
	while (is) {
		num_models++;
		int num_ops = Interpretation::parse_interpretation(is, domain_size, all_mt, ss, op_type, op_sym, true);
		if (num_ops <= 0)
			continue;
		ri_gen.calc_random_invariants(domain_size, random_list, trees, all_bin_function_mt, random_invariants);

		int invaraints_length = num_ops * Invariant::invariant_size + num_random;

        Invariant::calc_invariant_vec(domain_size, num_ops, all_mt, all_inv_vec, op_type, op_sym, no_basic_invariants);

        int** inv_vec = all_inv_vec[num_ops];
        for (int idx = 0; idx < num_random; idx++) {
        	std::vector<int>& inv = random_invariants[idx];
        	for (int jdx = 0; jdx < domain_size; jdx++)
        		inv_vec[jdx][idx] = inv[jdx];
        }
		Invariant::sort_invariant_vec(domain_size, invaraints_length, combo_inv_vec);
		Invariant::hash_key(domain_size, invaraints_length, combo_inv_vec, key);

		int compact_key;  // simple integer instead of a lengthy string as key
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
        num_models_processed++;
	}
	is.close();
	return num_models_processed;
}


/*
 * This builds all buckets using basic invariants and a list of random invariants
 */
int Buckets::build_buckets(int domain_size, int num_models, int num_ops, int* combo_inv_vec[], const std::vector<int>& random_list,
		std::vector<std::string>& models, InvariantsStore& inv_store, std::vector<std::vector<std::string>>& interps)
{
	int invariants_length = num_ops * Invariant::invariant_size + random_list.size();
	std::string key;
	int next_key = 0;
	std::unordered_map<std::string, int> buckets;
	for (int model_num=0; model_num<num_models; model_num++) {
		inv_store.retrieve_invariants(model_num, num_ops, combo_inv_vec, random_list);
		Invariant::sort_invariant_vec(domain_size, invariants_length, combo_inv_vec);
		Invariant::hash_key(domain_size, invariants_length, combo_inv_vec, key);

		int compact_key;  // simple integer instead of a lengthy string as key
		if (buckets.find(key) != buckets.end()) {
			compact_key = buckets[key];
		}
		else {
			compact_key = next_key;
			buckets[key] = compact_key;
			next_key++;
			interps.push_back(std::vector<std::string>());
		}
		interps[compact_key].push_back(models[model_num]);
	}
	return buckets.size();
}


/*
 * Given the basic invariants and a list of random invariants, find the score of the buckets generated by them
 */
int Buckets::score_invariants(int domain_size, int num_models, int num_ops, int* combo_inv_vec[], const std::vector<int>& random_list, InvariantsStore& inv_store)
{
	int invariants_length = num_ops * Invariant::invariant_size + random_list.size();
	std::string key;
	std::unordered_map<std::string, int> buckets;
	for (int model_num=0; model_num<num_models; model_num++) {
		inv_store.retrieve_invariants(model_num, num_ops, combo_inv_vec, random_list);
		Invariant::sort_invariant_vec(domain_size, invariants_length, combo_inv_vec);
		Invariant::hash_key(domain_size, invariants_length, combo_inv_vec, key);

		if (buckets.find(key) != buckets.end()) {
			buckets[key] += 1;
		}
		else {
			buckets[key] = 1;
		}
	}
	int score = 0;
	for (auto it : buckets) {
		score += (it.second - 1)*(it.second - 1);
	}
	//std::cerr << "min score " << score << " #buckets " << buckets.size() << std::endl;
	return score;
}


/*
 * Find the best score of the buckets generated by the basic invariants plus a subset (of fixed size) of random invariants
 */
int Buckets::find_best_random_invariants(int max_level, int domain_size, int num_models, int num_ops, int* combo_inv_vec[], int num_random,
		std::vector<int>& random_list, InvariantsStore& inv_store)
{
	if (num_random <= 0) {
		return -1;
	}

	int min_score = score_invariants(domain_size, num_models, num_ops, combo_inv_vec, std::vector<int>(), inv_store);
	std::cerr << "***************** find_best_random_invariants, initial score: " << min_score << std::endl;

	int level = -1;
	int visited[num_random] = {0};
	int best_invariant = -1;
	do {
		level++;
		best_invariant = -1;
		random_list.push_back(-1);
		for (int idx = 0; idx < num_random; idx++) {
			if (visited[idx] == 1)
				continue;
			random_list[level] = idx;
			int score = score_invariants(domain_size, num_models, num_ops, combo_inv_vec, random_list, inv_store);
			if (score < min_score) {
				min_score = score;
				best_invariant = idx;
			}
		}
		std::cerr << "***************** find_best_random_invariants, level: " << level << " trial score: " << min_score
				<< " best invariant: " << best_invariant << std::endl;
		random_list[level] = best_invariant;
		visited[best_invariant] = 1;
	}
	while (best_invariant > -1 && level < max_level - 1 && level < num_random - 1);
	if (best_invariant < 0) {
		level--;
		random_list.pop_back();
	}

	return level;
}


/*
 * Calculate all buckets without use invariantsStore.
 * This is for comparisons/testing
 */
int Buckets::no_savings(std::string& in_file, int domain_size, int& num_models, int starting_seed, int num_random,
		int max_sample_size, int sampling_frequency, std::vector<Tree>& trees,
		std::vector<int>& op_type, std::vector<std::string>& op_sym, std::vector<int**>& all_inv_vec, int* combo_inv_vec[],
		std::vector<std::vector<std::vector<int>>>& all_mt, std::vector<int>& all_bin_function_mt, std::vector<int>& all_bin_relation_mt,
		std::vector<std::string>& bin_function_op_sym, std::vector<std::string>& bin_relation_op_sym, std::vector<std::vector<std::string>>& interps)
{
	std::stringstream ss;
	int num_models_processed = 0;
	std::vector<std::vector<int>> random_invariants(num_random);
	RandomInvariants  ri_gen = RandomInvariants();
	num_models = -1;
	std::string key;
	int next_key = 0;
	std::unordered_map<std::string, int> buckets;
	std::ifstream is;
	is.open(in_file);
	while (is) {
		num_models++;
		bool to_process = (num_models_processed < max_sample_size) && (num_models % sampling_frequency == 0);
		int num_ops = Interpretation::parse_interpretation(is, domain_size, all_mt, ss, op_type, op_sym, to_process);
		if (num_ops <= 0)
			continue;
		ri_gen.init_random_invariants(domain_size, starting_seed, num_random,
					trees, op_type, op_sym, all_mt, all_bin_function_mt, all_bin_relation_mt,
					bin_function_op_sym, bin_relation_op_sym, random_invariants);
		ri_gen.calc_random_invariants(domain_size, num_random, trees, all_bin_function_mt, random_invariants);

		int invaraints_length = num_ops * Invariant::invariant_size + num_random;
        Invariant::calc_invariant_vec(domain_size, num_ops, all_mt, all_inv_vec, op_type, op_sym);
        int** inv_vec = all_inv_vec[num_ops];
        for (int idx = 0; idx < num_random; idx++) {
        	std::vector<int>& inv = random_invariants[idx];
        	for (int jdx = 0; jdx < domain_size; jdx++)
        		inv_vec[jdx][idx] = inv[jdx];
        }
		Invariant::sort_invariant_vec(domain_size, invaraints_length, combo_inv_vec);
		Invariant::hash_key(domain_size, invaraints_length, combo_inv_vec, key);

		int compact_key;  // simple integer instead of a lengthy string as key
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
        num_models_processed++;
	}
	is.close();
	return num_models_processed;
}


/*
 * This is for finding all buckets without random invariants
 */
int Buckets::bucketing(std::string& in_file, int domain_size, std::vector<std::vector<std::vector<int>>>& all_mt, int* combo_inv_vec[],
		std::vector<int**>& all_inv_vec, int& num_ops, std::vector<std::vector<std::string>>& interps)
{
	int num_models = -1;
	std::stringstream ss;
	int next_key = 0;
	std::string key;
	std::vector<int> op_type(max_num_functions);
	std::vector<std::string> op_sym;
	std::unordered_map<std::string, int> buckets;
	std::ifstream is;
	is.open(in_file);
	while (is) {
		num_models++;
		num_ops = Interpretation::parse_interpretation(is, domain_size, all_mt, ss, op_type, op_sym);
		if (num_ops <= 0)
			continue;

		int invaraints_length = num_ops*Invariant::invariant_size;
        Invariant::calc_invariant_vec(domain_size, num_ops, all_mt, all_inv_vec, op_type, op_sym);
        Invariant::sort_invariant_vec(domain_size, invaraints_length, combo_inv_vec);
        Invariant::hash_key(domain_size, invaraints_length, combo_inv_vec, key);

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
	is.close();
	return num_models;
}


