/*
 * Utils.h
 *
 *  Created on: Jul 14, 2021
 *      Author: uabcoimbra
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <string>

class Utils {
public:
	Utils();
	virtual ~Utils();

	static double get_wall_time();
	static void   save_statistics(const std::string& file_path, int num_blocks, int num_models, int num_ops,
			int sampling_frequency, int num_random_used, double inv_calc_time, double total_run_time, double max_time);
	static void   save_statistics(const std::string& file_path, int num_blocks, int num_models, int num_ops,
			double inv_calc_time, double total_run_time, double max_time);
};

#endif /* UTILS_H_ */
