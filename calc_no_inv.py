#!/usr/bin/python3
"""
This script runs the isomorphic models elimination for Mace4 outputs post interpformat - that is,
all unnecessary displays are removed - only the models are left.

This script does not use invariants

Remember to delete everything in the working directory (here the default is ./working_no_inv) before
starting the run.
"""

import os
from datetime import datetime
import time

import glob
import json
import pandas as pd
import subprocess as sp


exec = './splitModels'


def expand_file_path(path):
    file_path = [x for x in glob.glob(path)]
    if len(file_path) != 1:
        raise RuntimeError("too many/few files for wildcard path name")
    return file_path[0]

def read_algebras(path):
    """returns the details of the algebras in a list of dictionary.  Each algebra is an item in the list.
        The keys in the dictionary are attributes such as id, name, order, num_non_iso"""
    all_algebras = pd.read_excel(path, engine='openpyxl');
    return all_algebras.set_index("id").to_dict(orient="index")

def write_results(summary_file, stat):
    """write out the results"""
    with (open(summary_file, "a+")) as fp:
        fp.write(f"{stat['id']},\"{stat['name']}\",{stat['order']}")
        if stat['order'] == -1:
            fp.write(",,,,,,,,,\n")
        else:
            avg = (stat['num_non_iso']*1.0) / stat['num_blocks']
            fp.write(f",{stat['num_ops']},{stat.get('num_random', 0)},{stat['num_blocks']},{stat['num_non_iso']}")
            fp.write(f",{avg},{stat['num_models']}")
            fp.write(f",{stat['inv_calc_time']},{stat['total_run_time']},{stat['max_time']}\n")
    
    
def run_algebra(id, params, summary_file, model_path = "outputs", working_path = "working", num_random=50, max_level=10, minimum=-1, additional_params=""):
    order = params['order']
    print(f"Doing {params['name']}, {order}")

    if order > 0:
        model_file = params['model_file']
        if model_file == 'default':
            model_file = f"{id}_*_{order}.out"
        output_file_prefix = params['output_file_prefix']
        if output_file_prefix == "default":
            output_file_prefix = f"{working_path}/{id}_non_iso_models_{order}_"
        statistics_file = params['statistics_file']
        if statistics_file == "default":
            statistics_file = f"{working_path}/{id}_statistics_{order}.json"
        min_num_models_in_file = params["min_num_models_in_file"]
        sample_frequency = 1
        num_models = params['num_models']
        if num_models > 1000000:
            sample_frequency = 1000
        elif num_models > 100000:
            sample_frequency = 200
        elif num_models > 10000:
            sample_frequency = 20
        elif num_models > 5000:
            sample_frequency = 10
        else:
            sample_frequency = 1
        
        if minimum > 0:
            min_num_models_in_file = minimum
        elif min_num_models_in_file == -1:
            if num_models > 1000000:
                min_num_models_in_file = 1000
            elif num_models > 100000:
                min_num_models_in_file = 100
            elif num_models > 10000:
                min_num_models_in_file = 100
            elif num_models > 5000:
                min_num_models_in_file = 10
            else:
                min_num_models_in_file = 1

        num_models = params['num_models']
        sample_size = num_models // sample_frequency + 2
        
        model_file_path = expand_file_path(f"{model_path}/{model_file}")
        run_params = f'-d{order} -f{params["filter"]} -m{min_num_models_in_file} ' + \
                     f'-o{output_file_prefix} -t{statistics_file} -u{num_models} ' + \
                     f'-s{sample_frequency} -r{num_random} -l{max_level} -x{sample_size} -i"{model_file_path}" ' + \
                     additional_params
        cmd = f"{exec} {run_params}"
        print(cmd)
        cp = sp.run(cmd, capture_output=True, text=True, check=False, shell=True)
        
        with (open(f"{working_path}/error.out", "a+")) as fp:
            fp.write(f"{cmd}\n\n")
            fp.write(cp.stderr);
            fp.write("\n\n");

        with (open(statistics_file)) as fp:
            statistics = json.load(fp)
        
        num_non_iso = 0
        for file_path in glob.glob(f'{output_file_prefix}*.out.f'):
            with (open(file_path)) as fp:
                for line in fp:
                    if line.startswith('interpretation'):
                        num_non_iso += 1
        statistics['num_non_iso'] = num_non_iso
    else:
        statistics = dict()
        
    statistics['order'] = order
    statistics['name'] = params['name']
    statistics['id'] = id
    write_results(summary_file, statistics)
        

def run_all():
    all_algebras = read_algebras("algebras.xlsx")
    os.makedirs("outputs", exist_ok = True)
    
    # specifies number of random invariants to generate, maximum number of random invariants to use,
    # minimum number of models in a file (-1 for default - to be calculated, and whether basic invariants are not use: "-n".
    summary_file = "outputs/summary_no_inv.csv"
    working_dir = "working_no_inv"
    num_random = 0
    max_level = 20
    minimum = 1        # -1 for default, to be calculated
    add_param = "-n"    # "-n" for no basic invariants, -p for parallel
    
    os.makedirs(working_dir, exist_ok = True)
    
    print(f'Started at {datetime.now()} with num_random {num_random}, max_level {max_level}, and additional param {add_param}')
    
    with (open(summary_file, "w+")) as fp:
       fp.write('"id","name","order","#operations","#random invariants","#blocks","#non-isomorphic models","avg #non-iso models/block","#Mace4 outputs","invariants calc time (sec)", "total run time (sec)","max time (sec)"\n')
    for id, params in all_algebras.items():
        # params["filter"] = "isofilter2"
        if id in [8, 56]:
            params['order'] = -1
        print(datetime.now())
        run_algebra(id, params, summary_file, "new_outputs", working_dir, num_random, max_level, minimum, add_param)

    print(f'Finished at {datetime.now()}')

if __name__ == '__main__':
    run_all()
