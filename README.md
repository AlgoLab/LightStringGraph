LightStringGraph
================

Lightweight String Graph Construction

At this stage this software is a prototype/proof of concept and only builds the overlap graph of a given dataset.

### Building and running LighStringGraph

The software is composed by two different tools `lsg` (light (string) overlap grap) and `redbuild` (string graph build).

To build both lsg and redbuild simply move in the root directory of the project and run `make lsg redbuild`.

If you want to try out the software on some simulated reads follow this steps:

1. download and compile [BEETL](https://github.com/AlgoLab/BEETL) (versio 0.9.0 fixed to correctly compute LCP)
2. Given a FASTA file `a.fa` (all the reads must have the same size) build the BWT with `beetl bwt -i a.fa -o <BWTPrefix> -f ascii`
3. run LightStringGraph `./bin/lsg -B <BWTPrefix> -T <Tau> -C <CycNum>` where `<Tau>` is the minimum overlap between reads and `<CycNum> >= <reads length> - <Tau>`.
4. run redbuild `./bin/redbuild -b <BWTPrefix> -r a.fa -m <CycNum>+1 -l <ReadsLength>` and redirect STDOUT (the string graph in the [ASQG format](https://github.com/jts/sga/wiki/ASQG-Format)) to a file (you can compress it on the fly).
