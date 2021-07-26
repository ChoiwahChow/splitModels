/*
 * RandomInvariants.cpp
 *
 *  Created on: Jul 6, 2021
 *      Author: uabcoimbra
 */

#include <iostream>
#include "Invariant.h"
#include "InvariantsStore.h"

InvariantsStore::InvariantsStore(int ds, int no, int n_random, int nm):
	domain_size(ds), num_op(no), num_random(n_random), num_models(nm),
	basic_invariants_size(num_op*Invariant::invariant_size), all_invariants_size(basic_invariants_size+num_random),
	invariant_vecs(nullptr), model_size(domain_size * all_invariants_size), next_free_pos(0)
{
	invariant_vecs = new int[num_models * model_size];
	std::fill(invariant_vecs, invariant_vecs+(num_models * model_size), 0);
}

/*
 * Extract all invariants from the basic invariants, then all the random invariants (a vector of invariants,
 * each of which is for every domain element, and store them in a linear array.
 * all_inv_vec contains only basic invariants
 */
bool
InvariantsStore::save_invariants(int num_ops, std::vector<int**>& all_inv_vec, const std::vector<int*>& all_random_inv)
{
	if (next_free_pos >= num_models)  // no more space
		return false;

	int* invar = &invariant_vecs[next_free_pos * model_size];
	std::fill(invar, invar+model_size, 0);

	// copy for each domain element
	for (int idx=0; idx < domain_size; idx++) {
		int domain_element_pos = idx * all_invariants_size;
		// first copy the basic invariants of a domain element
		for (int mdx=0; mdx < num_ops; mdx++) {
			int** inv_vec = all_inv_vec[mdx];
			for (size_t jdx=0; jdx < Invariant::invariant_size; jdx++) {
				invar[domain_element_pos + mdx*Invariant::invariant_size + jdx] = inv_vec[idx][jdx];
			}
		}
		// then copy the random invariants of a domain element
		int current_basic_invariants_size = num_ops * Invariant::invariant_size;
		for (size_t kdx=0; kdx < all_random_inv.size(); kdx++) {
			invar[domain_element_pos + current_basic_invariants_size + kdx] = all_random_inv[kdx][idx];
		}
	}

	next_free_pos++;
	return true;
}

/*
 * Reverse of save_invariant: from the linear array, extract the invariants and send them back.
 */
bool
InvariantsStore::retrieve_invariants(int model_num, int num_ops, int* combo_inv_vec[], const std::vector<int>& random_list) const
{
	if (model_num >= next_free_pos || model_num < 0)  // out of bounds
		return false;

	int* invar = &invariant_vecs[model_num * model_size];

	int current_basic_invariants_size = num_ops * Invariant::invariant_size;
	for (int idx=0; idx < domain_size; idx++) {
		int domain_element_pos = idx * all_invariants_size;
		// first copy the basic invariants
		for (int jdx=0; jdx < current_basic_invariants_size; jdx++) {
			combo_inv_vec[idx][jdx] = invar[domain_element_pos+jdx];
		}
		// then copy the random invariants
		for (size_t kdx=0; kdx < random_list.size(); kdx++) {
			combo_inv_vec[idx][current_basic_invariants_size+kdx] = invar[domain_element_pos + current_basic_invariants_size + random_list[kdx]];
		}
	}
	/* debugging only
	std::cerr << "**retrieve_invariants, random_invariants**\n";
	for( int idx=0; idx < domain_size; idx++) {
		for (size_t jdx = 0; jdx < basic_invariants_size+random_list.size(); jdx++)
			std::cerr << combo_inv_vec[idx][jdx] << " ";
		std::cerr << "\n";
	}
	std::cerr << "********" << std::endl;
	*/

	return true;
}

InvariantsStore::~InvariantsStore() {
	delete invariant_vecs;
}



