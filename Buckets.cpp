/*
 * Buckets.cpp
 *
 *  Created on: Jul 14, 2021
 *      Author: Choiwah Chow
 */

#include <istream>
#include <fstream>
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
 * This is for computing all invariants: basic invariants and the all random invariants.
 */
int Buckets::calc_all_invariants(std::string& in_file, int domain_size, int& num_models, int starting_seed, int num_random,
		int max_sample_size, int sampling_frequency, std::vector<Tree>& trees, std::vector<int>& op_type, std::vector<std::string>& op_sym,
		std::vector<std::vector<std::vector<int>>>& all_mt, std::vector<int>& all_bin_function_mt, std::vector<int>& all_bin_relation_mt,
		std::vector<int>& all_unary_function_mt, std::vector<std::string>& bin_function_op_sym, std::vector<std::string>& bin_relation_op_sym,
		std::vector<std::string>& unary_function_op_sym, std::vector<std::vector<std::vector<int>>>& all_random_invariants,
		std::vector<std::vector<std::vector<int>>>& all_inv_vec, bool no_basic_invariants)
{
	std::stringstream ss;
	int num_models_processed = 0;
	RandomInvariants  ri_gen = RandomInvariants();
	num_models = -1;
	std::ifstream is;
	is.open(in_file);
	while (is) {
		num_models++;
		bool to_process = (num_models_processed < max_sample_size) && (num_models % sampling_frequency == 0);
		int num_ops = Interpretation::parse_interpretation(is, domain_size, all_mt, ss, op_type, op_sym, false, to_process);

		if (num_ops <= 0)
			continue;
		all_random_invariants.push_back(std::vector<std::vector<int>>(num_random));
		ri_gen.init_random_invariants(domain_size, starting_seed, num_random, trees, op_type, op_sym, all_mt, all_bin_function_mt,
				all_bin_relation_mt, all_unary_function_mt, bin_function_op_sym, bin_relation_op_sym, unary_function_op_sym,
				all_random_invariants[num_models_processed]);
		ri_gen.calc_random_invariants(domain_size, num_random, trees, all_bin_function_mt, all_random_invariants[num_models_processed]);

		all_inv_vec.push_back(std::vector<std::vector<int>>());
		if (no_basic_invariants)
			num_ops = 0;
		Invariant::calc_invariant_vec(domain_size, num_ops, all_mt, all_inv_vec[num_models_processed], op_type, op_sym);
        num_models_processed++;
	}
	is.close();
	return num_models_processed;
}


void Buckets::hash_model(std::unordered_map<std::string, int>& blocks, const std::string& model_str,
		int& next_key, std::vector<std::vector<int>>& all_inv_vec, std::vector<std::vector<std::string>>& interps)
{
	std::string key;

	Invariant::sort_invariant_vec(all_inv_vec);
	Invariant::hash_key(all_inv_vec, key);

	int compact_key;  // simple integer instead of a lengthy string as key, so that strings can be hashed into array/vector<int>
	if (blocks.find(key) != blocks.end()) {
		compact_key = blocks[key];
	}
	else {
		compact_key = next_key;
		next_key++;
		blocks[key] = compact_key;
		interps.push_back(std::vector<std::string>());
	}
	interps[compact_key].push_back(model_str);
}

/*
 * This is for computing all invariants: basic invariants and the subset of best random invariants,
 * no need to save the invariants
 */
int Buckets::calc_selected_invariants(std::string& in_file, int domain_size, int& num_models, const std::vector<int>& random_list,
		std::vector<Tree>& trees, std::vector<int>& op_type, std::vector<std::string>& op_sym,
		std::vector<std::vector<std::vector<int>>>& all_mt, std::vector<int>& all_bin_function_mt, std::vector<int>& all_bin_relation_mt,
		std::vector<std::vector<std::string>>& interps, bool no_basic_invariants)
{
	std::stringstream ss;
	int num_models_processed = 0;
	std::vector<std::vector<int>> random_invariants(random_list.size());
	std::vector<std::vector<int>> inv_vec;  // vector of all invariant vectors of a model, one for each domain element
	int basic_invariant_length = -1;
	RandomInvariants  ri_gen = RandomInvariants();
	std::unordered_map<std::string, int> blocks;
	num_models = -1;
	std::string key;
	int next_key = 0;
	int num_random = random_list.size();
	std::ifstream is;
	is.open(in_file);

	while (is) {
		num_models++;
		int num_ops = Interpretation::parse_interpretation(is, domain_size, all_mt, ss, op_type, op_sym);
		if (num_ops <= 0)
			continue;
		ri_gen.calc_random_invariants(domain_size, random_list, trees, all_bin_function_mt, random_invariants);

		if (no_basic_invariants)
			num_ops = 0;
		Invariant::calc_invariant_vec(domain_size, num_ops, all_mt, inv_vec, op_type, op_sym);
		if (basic_invariant_length < 0) {
			basic_invariant_length = inv_vec[0].size();
			for (int el = 0; el < domain_size; el++) {
				for (size_t idx = 0; idx < random_list.size(); ++idx)
					inv_vec[el].push_back(0);
			}
		}
        for (int idx = 0; idx < num_random; idx++) {
        	std::vector<int>& inv = random_invariants[idx];
        	for (int jdx = 0; jdx < domain_size; jdx++)
        		inv_vec[jdx][idx+basic_invariant_length] = inv[jdx];
        }
        hash_model(blocks, ss.str(), next_key, inv_vec, interps);
		ss.str("");
        num_models_processed++;
	}
	is.close();
	return num_models_processed;
}


void Buckets::copy_last_invariant(int domain_size, int base_invariants_length, std::vector<std::vector<std::vector<int>>>& all_inv_vec,
		const std::vector<std::vector<std::vector<int>>>& all_random_invariants, int pos, int best_invariant)
{
	int num_models = all_inv_vec.size();
	for (int model_num = 0; model_num < num_models; model_num++) {
		const std::vector<std::vector<int>>& random_invariants = all_random_invariants[model_num];
		std::vector<std::vector<int>>& combined_invariants = all_inv_vec[model_num];
		for (int el = 0; el < domain_size; ++el)
			combined_invariants[el][base_invariants_length+pos] = random_invariants[best_invariant][el];
	}
}


/*
 * Given the basic invariants and a list of random invariants, find the score of the buckets generated by them
 * Assume: The "all_inv_vec" already has all the random invariants in the random_list, except for the very last one.
 * This last one will be added to "all_inv_vec" before calculating the score, and in the next round, we don't need
 * to add this invariant again.
 */
int Buckets::score_invariants(int domain_size, int base_invariants_length, std::vector<std::vector<std::vector<int>>>& all_inv_vec,
		std::vector<std::vector<std::vector<int>>>& all_random_invariants, const std::vector<int>& random_list)
{
	std::string key;
	std::unordered_map<std::string, int> blocks;
	size_t num_models = all_inv_vec.size();
	size_t random_pos = random_list.size();
	for (size_t model_num = 0; model_num < num_models; model_num++) {
		std::vector<std::vector<int>>& random_invariants = all_random_invariants[model_num];
		std::vector<std::vector<int>>& combined_invariants = all_inv_vec[model_num];
		if (random_pos > 0) {
			if (combined_invariants[0].size() < base_invariants_length + random_pos) {
				for (int el = 0; el < domain_size; ++el)
					combined_invariants[el].push_back(0);
			}
			for (int el = 0; el < domain_size; ++el) {
				combined_invariants[el][base_invariants_length+random_pos-1] = random_invariants[random_list[random_pos-1]][el];
			}
		}

		Invariant::sort_invariant_vec(combined_invariants);
		Invariant::hash_key(combined_invariants, key);

		if (blocks.find(key) != blocks.end()) {
			blocks[key] += 1;
		}
		else {
			blocks[key] = 1;
		}
	}
	int score = 0;
	for (auto it : blocks) {
		score += (it.second - 1)*(it.second - 1);
	}
	//if (random_pos > 0)
	//	std::cerr << "min score " << score << " #blocks " << blocks.size() << " for invariant " << random_list[random_pos-1] << std::endl;
	return score;
}


/*
 * Find the best score of the buckets generated by the basic invariants plus a subset (of fixed size) of random invariants
 * random_list is output and must be empty on input.
 */
int Buckets::find_best_random_invariants(int max_level, int domain_size, std::vector<std::vector<std::vector<int>>>& all_inv_vec,
		std::vector<std::vector<std::vector<int>>>& all_random_invariants, std::vector<int>& random_list)
{
	if (all_random_invariants.size() <= 0 || all_random_invariants[0].size() <= 0) {
		return -1;
	}
	int num_random = all_random_invariants[0].size();
	int base_invariants_length = all_inv_vec[0][0].size();
	int min_score = score_invariants(domain_size, base_invariants_length, all_inv_vec, all_random_invariants, random_list);
	// std::cerr << "***************** find_best_random_invariants of " << all_random_invariants[0].size() << " invariants, initial score: " << min_score << std::endl;

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
			int score = score_invariants(domain_size, base_invariants_length, all_inv_vec, all_random_invariants, random_list);
			if (score < min_score) {
				min_score = score;
				best_invariant = idx;
			}
		}
		//std::cerr << "***************** find_best_random_invariants, level: " << level << " trial score: " << min_score
		//		<< " best invariant: " << best_invariant << std::endl;
		random_list[level] = best_invariant;

		if (best_invariant > -1) {
			copy_last_invariant(domain_size, base_invariants_length, all_inv_vec, all_random_invariants, level, best_invariant);
			visited[best_invariant] = 1;
		}
	}
	while (best_invariant > -1 && level < max_level - 1 && level < num_random - 1);

	if (best_invariant < 0) {
		level--;
		random_list.pop_back();
	}

	return level;
}

/*
 * This is for finding all buckets with random invariants
 */
std::vector<int> Buckets::bucketing(std::string& in_file, int domain_size, int starting_seed, int num_random, int max_sample_size,
		int sampling_frequency, int max_level, std::vector<std::vector<std::string>>& interps, bool no_basic_invariants)
{
	int num_models = -1;
	std::vector<int> op_type;
	std::vector<std::string> op_sym;

	std::vector<Tree> trees(num_random);
	std::vector<int> all_bin_function_mt;
	std::vector<std::string> bin_function_op_sym;
	std::vector<int> all_bin_relation_mt;
	std::vector<std::string> bin_relation_op_sym;
	std::vector<int> all_unary_function_mt;
	std::vector<std::string> unary_function_op_sym;

	std::vector<std::vector<std::vector<int>>> all_mt;
	std::vector<std::vector<std::vector<int>>> all_random_invariants;
	std::vector<std::vector<std::vector<int>>> all_inv_vec;
	std::vector<int> random_list;

	int actual_sample_size = calc_all_invariants(in_file, domain_size, num_models, starting_seed, num_random, max_sample_size,
			sampling_frequency, trees, op_type, op_sym, all_mt, all_bin_function_mt, all_bin_relation_mt, all_unary_function_mt,
			bin_function_op_sym, bin_relation_op_sym, unary_function_op_sym, all_random_invariants, all_inv_vec, no_basic_invariants);

	find_best_random_invariants(max_level, domain_size, all_inv_vec, all_random_invariants, random_list);

	interps.reserve(num_models);
	int num_models_processed = calc_selected_invariants(in_file, domain_size, num_models, random_list,
				trees, op_type, op_sym, all_mt, all_bin_function_mt, all_bin_relation_mt, interps, no_basic_invariants);

	std::vector<int> return_val;
	return_val.push_back((int)op_sym.size());
	return_val.push_back(num_models_processed);
	return_val.push_back((int)random_list.size());
	return_val.push_back(actual_sample_size);
	return return_val;
}

/*
 * This is for finding all buckets without random invariants
 */
int Buckets::bucketing(std::string& in_file, int domain_size, int& num_ops, int num_input_models, std::vector<std::vector<std::string>>& interps)
{
	int num_models = -1;
	std::stringstream ss;
	int next_key = 0;
	std::string key;
	std::vector<int> op_type;
	std::vector<std::string> op_sym;
	std::unordered_map<std::string, int> blocks;
	std::ifstream is;
	is.open(in_file);
	std::vector<std::vector<std::vector<int>>> all_mt;
	std::vector<std::vector<int>> inv_vec;
	interps.reserve(num_input_models);
	while (is) {
		num_models++;
		num_ops = Interpretation::parse_interpretation(is, domain_size, all_mt, ss, op_type, op_sym);
		if (num_ops <= 0)
			continue;
        Invariant::calc_invariant_vec(domain_size, num_ops, all_mt, inv_vec, op_type, op_sym);
        hash_model(blocks, ss.str(), next_key, inv_vec, interps);
		ss.str("");
	}
	num_ops = op_sym.size();
	return num_models;
}


