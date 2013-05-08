#ifndef TYPES_H
#define TYPES_H

#define BUFFERSIZE 1024

// Position on a BWT
typedef unsigned int BWTPosition; // 2^32 positions = ~ 4 billion bases

// Nucleotide counter
typedef unsigned int NucleoCounter;

// Possible Nucleotide in sequence
enum Nucleotide
  {
    BASE_$,
    BASE_A,
    BASE_C,
    BASE_G,
    BASE_T,
    BASE_N,
    BASE_Z,
    NOT_IN_ALPHABET,
    ALPHABET_SIZE
  };

#endif
