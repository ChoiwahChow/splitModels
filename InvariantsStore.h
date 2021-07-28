/*
 * RandomInvariants.h
 *
 *  Created on: Jul 6, 2021
 *      Author: uabcoimbra
 */

#ifndef INVARIANTSSTORE_H_
#define INVARIANTSSTORE_H_

#include <vector>

class InvariantsStore {
private:
	int  domain_size;
	int  num_op;
	int  num_random;
	int  num_models;
	size_t basic_invariants_size;
	size_t all_invariants_size;

	/*
	 * invariant_vecs holds all invariants for all models.
	 * It is a 1-dimensional array of int divided into segments, one for each model (total: num_models).
	 * Within each segment, it is divided into invarients, each invarient has a value for each domain element;
	 * So invariant_vecs:  model1 model2 mode3.... (for num_models)
	 * Then domain element within each model, each occupies invariant_size * num_op integers
	 * With the space allocated for each domain elements, fill in invariant1 invariant2 ...
	 * Each number in this array is an invariant for a domain element
	 * Note that the total number of invariants equals the number of random invariants plus the Invariant::Invarient_size fixed invariants
	 */
	int* invariant_vecs;
	int  model_size;   // number of int's needed to store one model's invariant vectors
	int  next_free_pos;
public:
	InvariantsStore(int ds, int num_op, int num_random, int nm);
	virtual ~InvariantsStore();

	bool save_invariants(int num_ops, std::vector<int**>& all_inv_vec, const std::vector<std::vector<int>>& random_inv);
	bool retrieve_invariants(int model_num, int num_ops, int* combo_inv_vec[], const std::vector<int>& random_list) const;
	void reset_storage() {next_free_pos = 0;};
	int  model_count() {return next_free_pos;};
};

#endif /* INVARIANTSSTORE_H_ */
