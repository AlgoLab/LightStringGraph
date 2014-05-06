// -*- c++ -*-

#ifndef TYPES_H
#define TYPES_H

#include <map>
#include <vector>
#include <cstdint>

#ifdef STXXL
#include <stxxl.h>
#endif

using std::vector;

// #define JoinedQIntervalManager IntervalManager< JoinedQInterval >
// #define EdgeJoinedQIntervalManager IntervalManager< EdgeInterval >
#define QIntervalManager IntervalManager< QInterval >
#define SameLengthArcIntervalManager IntervalManager< ArcInterval >
#define SameLengthEdgeLabelIntervalManager IntervalManager< EdgeLabelInterval >
#define BasicArcIntervalManager MultiIntervalManager<ArcInterval>

// Position on a BWT
typedef uint64_t BWTPosition;

// Possible value of LCP
typedef uint32_t LCPValue;

// Edge Length
typedef unsigned int EdgeLength; // should be enough for short reads

// Nucleotide counter
typedef uint64_t NucleoCounter;

// Possible Nucleotide in sequence
enum Nucleotide
  {
    BASE_$,
    BASE_A,
    BASE_C,
    BASE_G,
    BASE_N,
    BASE_T,
    BASE_Z,
    //    NOT_IN_ALPHABET,
    ALPHABET_SIZE
  };

// As defined in BEETL/src/shared/Tools.hh (ElementType)

// Type to represent: Number of sequences
// below limits to 4 billion reads max - change to uint64_t for more
typedef uint32_t SequenceNumber;

// Type to represent: Sequence length (in biologic case 100)
typedef uint32_t SequenceLength;

struct GSAEntry
{
  SequenceLength sa; // suffix array position
  SequenceNumber numSeq; // sequence number
};

// STXXL placeholder
//typedef stxxl::vector< BWTPosition, PAGE_SIZE, stxxl::lru_pager< PAGE_NUM >, BLOCK_SIZE * 1024 * 1024, stxxl::RC > BWTPExtVect;
//typedef stxxl::vector< SequenceNumber, PAGE_SIZE, stxxl::lru_pager< PAGE_NUM >, BLOCK_SIZE * 1024 * 1024, stxxl::RC > SeqNExtVect;
typedef std::vector< BWTPosition > BWTPExtVect;
typedef std::vector< SequenceNumber > SeqNExtVect;


// Other types

typedef std::vector<SequenceNumber> ReadSet;


// LEGACY
// temporary
// struct SGEdge
// {
//   unsigned int first_read;
//   unsigned int second_read;
//   EdgeLength len;
// };

// typedef std::map< unsigned int, SGEdge* > SGraph;
// typedef std::map< unsigned int, vector< unsigned int > > Precedencies;

#endif
