#ifndef TYPES_H
#define TYPES_H

#include <map>
#include <vector>

using std::vector;

#define BUFFERSIZE 1024
#define IM_BUFFERSIZE 256

#define TAU 50
#define CYCNUM 160

#define JoinedQIntervalManager IntervalManager< JoinedQInterval >
#define EdgeJoinedQIntervalManager IntervalManager< EdgeInterval >

// Position on a BWT
typedef unsigned int BWTPosition; // 2^32 positions = ~ 4 billion bases

// Edge Length
typedef unsigned int EdgeLength; // should be enough for short reads

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
    //    NOT_IN_ALPHABET,
    ALPHABET_SIZE
  };

// As defined in BEETL/src/shared/Tools.hh (ElementType)
struct GSAEntry
{
  unsigned int sa; // suffix array position
  unsigned int numSeq; // sequence number
};

// temporary
struct SGEdge
{
  unsigned int first_read;
  unsigned int second_read;
  EdgeLength len;
};

typedef std::map< unsigned int, SGEdge* > SGraph;
typedef std::map< unsigned int, vector< unsigned int > > Precedencies;

#endif
