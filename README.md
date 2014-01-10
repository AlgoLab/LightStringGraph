LightStringGraph
================

Lightweight String Graph Construction

At this stage this software is a prototype/proof of concept.

### Building and running LighStringGraph
If you want to try out software follow this steps:

1. download and compile [BEETL](https://github.com/BEETL/BEETL) (tested with version 0.4.0, set to 1 the flag `BUILD_SA` in src/shared/Tools.hh)
2. given your FASTA file `a.fa` create a new FASTA file `b.fa` that contains the reversed reads of `a.fa` (if read `x` is at position `N` in `a.fa`, read `rev(x)` should be in position `N` in `b.fa`)
3. build the BWT of a.fa `beetl bwt -i a.fa -o BWT -f ascii`
4. build the BWT of b.fa `beetl bwt -i b.fa -o revBWT -f ascii`
5. `make all` LighStringGraph
6. rebuild GSA[ $ ] `./bin/bgsa -i BWT.pairSA -o newSA.pairSA`
7. run LightStringGraph `./bin/stringGraph -B BWT-B0 -R revBWT-B0 -G newSA.pairSA`

stringGraph outputs (STDOUT) a json-like structure of "edges". Every edge has 3 fields:

1. `from`: source read ID (read position in `a.fa`)
2. `to`: destination read ID (read position in `a.fa`)
3. `length`: number of bases in the second read (the one pointed by `to`) that aren't in overlap with the first read


### NOTE
At this stage LightStringGraph works only with simulated genomic reads from the same strand and with only forward orientation. As stated before this is a proof of concept and will be improved later on.
