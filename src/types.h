#ifndef TYPES_H
#define TYPES_H

// Position on a BWT
typedef unsigned int BWTPosition; // 2^32 positions = ~ 4 billion bases

// Possible Nucleotide in sequence
enum Nucleotide
  {
    BASE_A,
    BASE_C,
    BASE_G,
    BASE_T,
    BASE_N,
    ALPHABET_SIZE
  };

#endif
