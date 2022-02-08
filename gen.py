#!/usr/bin/python3
"""
This script runs the isomorphic models elimination for Mace4 outputs post interpformat - that is,
all unnecessary displays are removed - only the models are left.

Remember to delete everything in the working directory (here the default is ./working) before
starting the run.
"""

import os
from datetime import datetime
import time

import glob
import json
import pandas as pd
import subprocess as sp


def read_algebras(path):
    """returns the details of the algebras in a list of dictionary.  Each algebra is an item in the list.
        The keys in the dictionary are attributes such as id, name, order, num_non_iso"""
    all_algebras = pd.read_excel(path, engine='openpyxl');
    return all_algebras.set_index("id").to_dict(orient="index")



def run_algebra(id, params, outputs_dir):
    order = params['order']
    name = params['name']
    print(f"Doing {id} {order}")

    if order > 0:
        for ifp in glob.glob(f'inputs/{id}_*.in'):
            in_file = ifp

        output_file_name = name.replace(" ", "_").replace("#", "").replace("-", "_") + f"_{order}.out"

        cmd = f"./bin/mace4 -n{order} -N{order} -m-1 -A1 -f {in_file} 2> {outputs_dir}/{output_file_name}"
        print(cmd)
        cp = sp.run(cmd, capture_output=True, text=True, check=False, shell=True)
        cp = sp.run(f"tail -50  {outputs_dir}/{output_file_name} | grep interp | tail -1", capture_output=True, text=True, check=False, shell=True)
        print(cp.stdout)
        

def run_all():
    all_algebras = read_algebras("algebras.xlsx")
    os.makedirs("outputs", exist_ok = True)
    
    print(f'Started at {datetime.now()}')
    
    for id, params in all_algebras.items():
        print(datetime.now())
        run_algebra(id, params, "outputs")

    print(f'Finished at {datetime.now()}')

if __name__ == '__main__':
    run_all()
