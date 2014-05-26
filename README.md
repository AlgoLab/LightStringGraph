LightStringGraph
================

Lightweight String Graph Construction

This software is in its initial stage of development.
Please contact [Marco Previtali](http://algolab.eu/people/previtali/) for any questions or use the [issue tracker](https://github.com/AlgoLab/LightStringGraph/issues) for reporting bugs.

### Building and running LightStringGraph

The software is composed of two different tools: `lsg` (light (string) overlap graph) and `redbuild` (string graph build).

To build both lsg and redbuild simply move in the root directory of the project and run `make lsg redbuild`.

If you want to try out the software on some (simulated or real) reads follow this steps:

1. download and compile [BEETL](https://github.com/AlgoLab/BEETL) (version 0.9.0 fixed to correctly compute LCP)
2. given a FASTA file `a.fa` (all the reads must have the same length), build the BWT with `beetl bwt -i a.fa -o <BWTPrefix> -f ascii`
3. run LightStringGraph `./bin/lsg -B <BWTPrefix> -T <Tau> -C <CycNum>` where `<Tau>` is the minimum overlap between reads and `<CycNum> >= <reads length> - <Tau>`.
4. run redbuild `./bin/redbuild -b <BWTPrefix> -r a.fa -m <CycNum>+1 -l <ReadLength>` and redirect STDOUT (the string graph in the [ASQG format](https://github.com/jts/sga/wiki/ASQG-Format)) to a file (you can compress it on the fly).
