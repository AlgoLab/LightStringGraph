// -*- c++ -*-

/**
 *
 *                     LightStringGraph
 *
 *            Lightweight String Graph Construction.
 *
 * Copyright (C) 2013, 2014 Stefano Beretta, Yuri Pirola, Marco Previtali
 *
 * Distributed under the terms of the GNU General Public License (or the Lesser
 * GPL).
 *
 * This file is part of LightStringGraph.
 *
 * LighStringGraph is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * LightStringGraph is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with LightStringGraph.  If not, see <http://www.gnu.org/licenses/>.
 *
 **/

#ifndef TYPES_H
#define TYPES_H

#include <map>
#include <vector>
#include <cstdint>

using std::vector;

// #define JoinedQIntervalManager IntervalManager< JoinedQInterval >
// #define EdgeJoinedQIntervalManager IntervalManager< EdgeInterval >
#define QIntervalManager IntervalManager< QInterval >
#define SameLengthArcIntervalManager IntervalManager< ArcInterval >
#define SameLengthEdgeLabelIntervalManager IntervalManager< EdgeLabelInterval >
#define BasicArcIntervalManager MultiIntervalManager<ArcInterval>

// As defined in BEETL/src/shared/Tools.hh (ElementType)

// Type to represent: Number of sequences
// below limits to 4 billion reads max - change to uint64_t for more
typedef uint32_t SequenceNumber;

// Type to represent: Sequence length (in biologic case 100)
typedef uint8_t SequenceLength;

struct GSAEntry
{
  SequenceLength sa; // suffix array position
//  SequenceNumber numSeq; // sequence number
};

// END of definition taken from BEETL

#define PRINT_SL( x ) static_cast<uint32_t>( (x) )

// Position on a BWT
typedef uint64_t BWTPosition;

// Possible value of LCP
typedef SequenceLength LCPValue;

// Edge Length
typedef SequenceLength EdgeLength; // should be enough for short reads

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

#define CHAR_BASE_$ '$'
#define CHAR_BASE_A 'A'
#define CHAR_BASE_C 'C'
#define CHAR_BASE_G 'G'
#define CHAR_BASE_N 'N'
#define CHAR_BASE_T 'T'
#define CHAR_BASE_Z 'Z'
#define CHAR_ALPHABET_SIZE '\0'
#define CHAR_BASE_A_LOW 'a'
#define CHAR_BASE_C_LOW 'c'
#define CHAR_BASE_G_LOW 'g'
#define CHAR_BASE_N_LOW 'n'
#define CHAR_BASE_T_LOW 't'
#define CHAR_BASE_Z_LOW 'z'



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
