/*
 * Buckets.h
 *
 *  Created on: Jul 14, 2021
 *      Author: uabcoimbra
 */

#ifndef BUCKETS_H_
#define BUCKETS_H_

#include <istream>
#include <string>
#include <vector>


class Buckets {
public:
	static const unsigned int max_num_functions = 108;

	Buckets();
	virtual ~Buckets();

	static int bucketing(std::istream& is, int domain_size, std::vector<int**>& all_mt, int* combo_inv_vec[],
			std::vector<int**>& all_inv_vec, std::vector<std::vector<std::string>>& interps);
};

#endif /* BUCKETS_H_ */
