/*
 * Invariant.h
 *
 *  Created on: May 1, 2021
 *      Author:
 */

#ifndef INVARIANT_H_
#define INVARIANT_H_

#include <string>
#include <vector>

class Invariant {
private:
	static int find_num_unique_el(const std::vector<int>& vec);
	static int find_num_unique_el(int s, const int vec[]);
	static int count_non_zero(int s, const int vec[]);
	static void add_space(std::vector<std::vector<int>>& all_inv_vec, int num_space);

	static int calc_binary_invariant_vec(const std::vector<std::vector<int>>& mt, std::vector<std::vector<int>>& inv_vec, int inv_pos);
	static int calc_binary_relation_invariant_vec(const std::vector<std::vector<int>>& mt, std::vector<std::vector<int>>& inv_vec, int inv_pos);
	static int calc_unary_invariant_vec(const std::vector<int>& mt, std::vector<std::vector<int>>& inv_vec, int inv_pos);
	static int calc_ternary_invariant_vec(const std::vector<int>& mt, std::vector<std::vector<int>>& inv_vec, int inv_pos);

	static int calc_binary_invariant_vec2(const std::vector<std::vector<int>>& mt, std::vector<std::vector<int>>& inv_vec, int inv_pos);

public:
	static const int ternary_inv_count = 1;
	static const int unary_inv_count = 4;
	static const int binary_inv_count = 10;
	static const int binary_relation_inv_count = 4;

	// mt - multiplication table
	static void calc_invariant_vec(int domain_size, int num_ops, std::vector<std::vector<std::vector<int>>>& all_mt,
			std::vector<std::vector<int>>& all_inv_vec, std::vector<int>& op_type, std::vector<std::string>& op_sym);
	static void hash_key(const std::vector<std::vector<int>>& all_inv_vec, std::string& key);
	static void sort_invariant_vec(std::vector<std::vector<int>>& all_inv_vec);
};




#endif /* INVARIANT_H_ */
