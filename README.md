LightStringGraph
================

Lightweight String Graph Construction

This software is in its initial stage of development.
Please contact [Marco Previtali](http://algolab.eu/people/previtali/) for any questions or use the [issue tracker](https://github.com/AlgoLab/LightStringGraph/issues) for reporting bugs.

### Building and running LightStringGraph

The software is composed of three different tools: `lsg` (light (string) overlap graph), `redbuild` (string graph build), and `graph2asqg` (native graph format to ASQG).

To build all the tools simply move in the root directory of the project and run `make all`.

If you want to try out the software on some (simulated or real) reads follow these steps:

0. given a FASTA file `a.fa` containing `n` reads produce another FASTA file `b.fa` containing `2n` reads s.t. reads between position `1` and `n` are the same as those in `a.fa` and reads between position `n+1` and `2n` are their reverse and complement (for `1 <= i <= n`, read in position `n+i` should be the reverse complement of read in position `i`)
1. download and compile [BEETL](https://github.com/AlgoLab/BEETL) (**please note that this is not the original repository**)
2. compile LightStringGraph `cd <LSGPATH> && make all`
3. build the BWT of `b.fa` with `beetl-bwt -i b.fa -o <BWTPrefix> --output-format=ASCII --generate-lcp --generate-end-pos-file`
4. run LightStringGraph `<LSGPATH>/bin/lsg -B <BWTPrefix> -T <Tau> -C <CycNum>` where `<Tau>` is the minimum overlap between reads and `<CycNum> >= <reads length> - <Tau>`
5. run redbuild `<LSGPATH>/bin/redbuild -b <BWTPrefix> -r b.fa -m <CycNum>+1`
6. optionally run graph2asqg `<LSGPATH>/bin/graph2asqg -b <BWTPrefix> -r b.fa -l <readsLength>` and redirect STDOUT (the string graph in the [ASQG format](https://github.com/jts/sga/wiki/ASQG-Format)) to a file (you can compress it on the fly).

**If `lsg` crashes and produces a logic error try to raise the limit on the maximum number of open file descriptors for the user running that command (for example, with the `bash` built-in `ulimit -n`) and delete all the `*.tmplsg.*` files before running `lsg` again.**
