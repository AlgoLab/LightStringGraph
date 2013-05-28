#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <cstdlib>
#include <vector>
#include <fstream>

#include "types.h"
#include "edge_joined_interval.h"
#include "joined_q_interval.h"

using std::ofstream;
using std::ifstream;
using std::vector;

// convert nucleotide (enum) to char
char ntoc ( Nucleotide base );

// convert char to nucleotide
Nucleotide cton ( char c );

// Check if there is some irreducible edge between the reads pointed
// by two Q-intervals
void checkIfIrreducible( SGraph&, Precedencies&, GSAEntry*, GSAEntry*, EdgeLength& );

ofstream& operator<<( ofstream&, const EdgeInterval& );

ifstream& operator>>( ifstream&, EdgeInterval* );

ofstream& operator<<( ofstream&, const JoinedQInterval& );

ifstream& operator>>( ifstream&, JoinedQInterval* ); 

#endif
