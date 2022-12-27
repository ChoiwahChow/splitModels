/*
 * IsoFilter.h
 *
 *  Created on: Jul 14, 2021
 *      Author: uabcoimbra
 */

#ifndef ISOFILTER_H_
#define ISOFILTER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

class IsoFilter {
private:
	static int find_slot(int num_threads);

public:
	IsoFilter();
	virtual ~IsoFilter();

	static int    run_filter(std::vector<std::string>& interps, const std::string& output_file_prefix,
			const std::string& mace_filter);

	static double run_filter(std::vector<std::vector<std::string>>& interps, const std::string& output_file_prefix,
			const std::string& mace_filter, unsigned int min_models_in_file, bool find_biggest_only, int num_threads);

	static double run_filter(const std::string& in_file, const std::string& filename, std::string& mace_filter);
};

#endif /* ISOFILTER_H_ */
