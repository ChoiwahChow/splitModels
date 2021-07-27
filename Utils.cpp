/*
 * Utils.cpp
 *
 *  Created on: Jul 14, 2021
 *      Author: uabcoimbra
 */

#include <time.h>
#include <sys/time.h>
#include <iostream>
#include <fstream>
#include "Utils.h"

Utils::Utils() {
	// TODO Auto-generated constructor stub

}

Utils::~Utils() {
	// TODO Auto-generated destructor stub
}

double Utils::get_wall_time()
{
    struct timeval time;
    if (gettimeofday(&time,NULL)){
        //  Handle error
        return 0;
    }
    return (double)time.tv_sec + (double)time.tv_usec * .000001;
}


void Utils::save_statistics(const std::string& file_path, int num_blocks, int num_models, int num_ops, int sampling_frequency,
		int num_random_used, double inv_calc_time, double total_run_time, double max_time)
{
	std::cerr << "Number of models: " << num_models;
	if (sampling_frequency > 0)
		std::cerr << ", sampling frequency: " << sampling_frequency;
	std::cerr << std::endl;
	std::cerr << "Number of operations: " << num_ops << "," << std::endl;
	std::cerr << "Number of blocks: " << num_blocks << " in " << inv_calc_time << " seconds." << std::endl;
	if (num_random_used >= 0)
		std::cerr << "Number of random invariants: " << num_random_used << std::endl;
	std::cerr << "Time for calculating invariants: " << inv_calc_time << std::endl;
	std::cerr << "Maximum Processing time " << max_time << std::endl;
	std::cerr << "Total run time: " << total_run_time << std::endl;

	if( file_path.empty() )
		return;
	std::ofstream ofs;
	ofs.open(file_path, std::ofstream::out);
	ofs << "{\n";
	ofs << "\"num_models\": " << num_models << "," << std::endl;
	ofs << "\"num_ops\": " << num_ops << "," << std::endl;
	if (sampling_frequency > 0)
		ofs << "\"sampling_frequency\": " << sampling_frequency << "," << std::endl;
	ofs << "\"num_blocks\": " << num_blocks << "," << std::endl;
	if (num_random_used >= 0)
		ofs << "\"num_random\": " << num_random_used << "," << std::endl;
	ofs << "\"inv_calc_time\": " << inv_calc_time << "," << std::endl;
	ofs << "\"max_time\": " << inv_calc_time << "," << std::endl;
	ofs << "\"total_run_time\": " << total_run_time << std::endl;
	ofs << "}" << std::endl;
	ofs.close();
}


void Utils::save_statistics(const std::string& file_path, int num_blocks, int num_models, int num_ops,
		double inv_calc_time, double total_run_time, double max_time)
{
	save_statistics (file_path, num_blocks, num_models, num_ops, -1, -1, inv_calc_time, total_run_time, max_time);
}
