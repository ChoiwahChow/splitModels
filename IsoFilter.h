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
public:
	IsoFilter();
	virtual ~IsoFilter();

	static double run_filter(const std::vector<std::vector<std::string>>& interps, const std::string& output_file_prefix,
			const std::string& mace_filter, unsigned int min_models_in_file, bool find_biggest_only, bool multiprocessing_on);
};

#endif /* ISOFILTER_H_ */
