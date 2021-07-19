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
	static int find_num_unique_el(int s, const int vec[]);
	static int count_non_zero(int s, const int vec[]);

	static void calc_ternary_invariant_vec(int domain_size, int* mt, int** inv_vec);
	static void calc_unary_invariant_vec(int domain_size, int* mt, int** inv_vec);
	static void calc_invariant_vec(int domain_size, int** mt, int** inv_vec);
	static void calc_relation_invariant_vec(int domain_size, int** mt, int** inv_vec);
public:
	static const int invariant_size = 10;

	static const int invariant_size_2  = 3;

	// mt - multiplication table
	static void calc_invariant_vec(int domain_size, int num_ops, std::vector<int**>& all_mt,
			std::vector<int**>& inv_vec, std::vector<int>& op_type, std::vector<std::string>& op_sym);
	static void hash_key(int domain_size, int num_invariants, int** all_inv_vec, std::string& key);
	static void sort_invariant_vec(int domain_size, int num_invariants, int** all_inv_vecc);
};




#endif /* INVARIANT_H_ */
