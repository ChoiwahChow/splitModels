# splitModels

The theoretical basis of this tool is given in

João Araújo, Choiwah Chow, Mikolás Janota. Boosting isomorphic model filtering with invariants. Constraints An Int. J. 27(3): 360-379 (2022)

## Compiling the project
This is a simple gnu C++ Makefile project, tested in the linux platform:

```text
make clean
make
```

The executable, splitModels, will be in the top directory as the source code files.  It can work with any program that filters isomorphic models given in `Mace4's` output format. It has been tested with `Mace4's isofilter`.

## Testing
To test splitModels, first generate a Mace4 output file, such as the models for semigroup of order 6.  A Mace4 input file for semigroup in available for testing:

```text
mkdir outputs
mace4 -n5 -N5 -f inputs/semi.in | interpformat > outputs/semi5.out
```

Next, use splitModels to split up the set of models into smaller partitions using invariant vectors, and also throw away isomorphic models:

```text
mkdir -p parallel working
./splitModels -d5 -f"../bin/isofilter -ignore_constants" -m100 -oworking/m_ -tworking/semi_statistics.json -i"./outputs/semi5.out" -p4
```
The required argument, "5", in this example, is the order of the semigroup.  The "parallel" directory is required to run the job, even if it is run in a single thread.

The outputs are in multiple files, all ending in `.f`, in the outputs folder.  To collect them into the file `outputs/non_iso5.out`:

```text
grep -v "isofilter" outputs/m*.f > outputs/non_iso5.out
```

The output folder should be cleaned up before the next run:

```text
rm outputs/m*.out
rm outputs/m*.f
```

## Input Options
```text
-d    the order of the algebra
-f    the isofilter - file path and input options
-m    minimum number (if there are more) of models for each call of the isofilter
-o    the working and outputs file prefix, including directory name. For example -oworking/m_ means all working files are prefixed with m_ in the "working" folder 
-t    the output summary .json file, e.g., -tworking/semi_stat.json
-i    the "input" file of models
-p    (optional) number of parallel processes, e.g., -p4 means use 4 parallel processes. Default: 1.
```

## Random invariants
Some additional input options are needed if random invariants are used.  The program creates a number (specified by `-r`) of invariants randomly, then take a sample of input models (1 out of a fixed number of models, specified by `-s`), but not more than a certain maximum number of models (specified by `-x`).
Then it applies the greedy algorithm by calculating a score for each random invariants using the sample. Take the best invariant.  Repeat the process with the rest of the invariants until the score does not improve, or the maximum level is reached, e.g., 5 invariants (specified by `-l`) have been picked. 

```text
-r    number of random invariants to try
-s    (optional) sampling frequency, e.g., -s100 means take 1 out of each 100 models as a sample. Default: 1
-x    (optional) max sample size. Default: 1000000
-l    (optional) max "level" of random invariants to try before stopping.  Default: 5.
```
Some of these parameters may not be readily available. In general, random invariants help to make sure "bad" cases don't affect the performance too much.
So, use the defaults if no better information is available.

Two sample scripts, `run.sh` and `run_random.sh`, are available in this repository.

