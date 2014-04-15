LightStringGraph
================

Lightweight String Graph Construction

At this stage this software is a prototype/proof of concept and only builds the overlap graph of a given dataset.

### Building and running LighStringGraph

The software is composed by two different tools `bgsa` (build general suffix array) and `lsg` (light (string) overlap grap).

To build both lsg and bgsa simply move in the root directory of the project and run `make all`.

If you want to try out the software on some simulated reads follow this steps:

1. download and compile [BEETL](https://github.com/BEETL/BEETL) (tested with version 0.4.0 and 0.9.0, set to 1 the flag `BUILD_SA` in src/shared/Tools.hh before compiling)
2. Given a FASTA file `a.fa` (all the reads must have the same size) build the BWT with `beetl bwt -i a.fa -o BWT -f ascii`
3. rebuild GSA `./bin/bgsa -i BWT.pairSA -o coBWT.pairSA -l <reads length>` (may take a while, not yet optimized)
4. run LightStringGraph `./bin/lsg -B BWT-B0 -G newSA.pairSA -T <Tau> -C <CycNum>` where `<Tau>` is the minimum overlap between reads and `<CycNum> >= <reads length> - <Tau>`.

`lsg` outputs (STDOUT) all the possible arcs as a csv, one arc per line. Every line is represented by a triplet of integers `<from>,<to>,<length>` where

1. `from` is the source read ID (read position in `a.fa`)
2. `to` is the destination read ID (read position in `a.fa`)
3. `length` is the length of the overlap between the reads `from` and `to`.
