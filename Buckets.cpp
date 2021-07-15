/*
 * Buckets.cpp
 *
 *  Created on: Jul 14, 2021
 *      Author: uabcoimbra
 */

#include <unordered_map>
#include "Interpretation.h"
#include "Invariant.h"
#include "Buckets.h"

Buckets::Buckets() {
	// TODO Auto-generated constructor stub

}

Buckets::~Buckets() {
	// TODO Auto-generated destructor stub
}


int Buckets::bucketing(std::istream& is, int domain_size, std::vector<int**>& all_mt, int* combo_inv_vec[],
		std::vector<int**>& all_inv_vec, std::vector<std::vector<std::string>>& interps)
{
	int num_models = -1;
	std::stringstream ss;
	int next_key = 0;
	std::string key;
	std::vector<int> op_type(max_num_functions);
	std::vector<std::string> op_sym;
	std::unordered_map<std::string, int> buckets;
	std::vector<int*> pre_calced;    // TODO fill it up
	while (is) {
		num_models++;
		int num_ops = Interpretation::parse_interpretation(is, domain_size, all_mt, ss, op_type, op_sym);
		if (num_ops <= 0)
			continue;

        Invariant::calc_invariant_vec(domain_size, num_ops, all_mt, all_inv_vec, op_type, op_sym, pre_calced);
        Invariant::sort_invariant_vec(domain_size, num_ops * Invariant::invariant_size, combo_inv_vec);
        Invariant::hash_key(domain_size, num_ops * Invariant::invariant_size, combo_inv_vec, key);

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
	return num_models;
}
