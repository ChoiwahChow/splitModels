# splitModels

## Compiling the project
This is a simple Makefile project:

```text
make clean
make
```

The executable, splitModels, will be in the top directory as the source code files.

## Testing
To test splitModels, first generate a Mace4 output file, such as the models for semigroup of order 6.  A Mace4 input file for semigroup in available for testing:

```text
mace4 -n6 -N6 -f inputs/semi.in | interpformat > outputs/semi6.out
```

Next, use splitModels to split up the models into smaller partitions using invariant vectors, and also throw away isomorphic models:

```text
cat outputs/semi6.out | splitModels 6 
```
The required argument, "6" in this example, is the order of the semigroup.

The outputs may be in multiple files in the outputs folder, but the files will all be ending in `.f`.  To collect them:

```text
grep -v "isofilter" outputs/m*.f > outputs/non_iso6.out
```

The output folder should be cleaned up before the next run:

```text
rm outputs\m*.out
rm outputs\m*.f
```
