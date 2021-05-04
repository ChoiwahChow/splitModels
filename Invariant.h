/*
 * Invariant.h
 *
 *  Created on: May 1, 2021
 *      Author:
 */

#ifndef INVARIANT_H_
#define INVARIANT_H_

#include <string>

class Invariant {
private:
	static int find_num_unique_el(int s, const int vec[]);
	static int count_non_zero(int s, const int vec[]);

public:
	static const int invariant_size = 10;         // number of invariants

	// mt - multiplication table
	static void calc_invariant_vec(int domain_size, int** mt, int** invariant_vec);
	static int  invariant_vec_length(int domain_size) { return invariant_size * domain_size; };
	static void hash_key(int domain_size, int** inv_vec, std::string& key);
};




#endif /* INVARIANT_H_ */
