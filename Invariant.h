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

public:
	static const int invariant_size = 10;         // number of invariants

	static const int invariant_size_2  = 3;

	// mt - multiplication table
	static void calc_invariant_vec(int domain_size, int num_binop, std::vector<int**> all_mt, std::vector<int**> all_inv_vec);
	static void calc_invariant_vec(int domain_size, int** mt, int** inv_vec);
	static void calc_relation_invariant_vec(int domain_size, int** mt, int** inv_vec);
	static int  invariant_vec_length(int domain_size) { return invariant_size * domain_size; };
	static void hash_key(int domain_size, int** inv_vec, std::string& key);
	static void hash_key(int domain_size, int num_binop, int** all_inv_vec, std::string& key);
	static void sort_invariant_vec(int domain_size, int num_binop, int** all_inv_vecc);

	static void calc_cross_invariant_vec(int domain_size, int num_binop, std::vector<int**> all_mt, std::vector<int**> all_inv_vec);
	static void calc_2_invariant_vec(int domain_size, int** mt1, int** mt2, int** inv_vec);
	static void sort_cross_invariant_vec(int domain_size,  int** combo_inv_vec);
	static void cross_hash_key(int domain_size, int** combo_in_vec, std::string& key);
};




#endif /* INVARIANT_H_ */
