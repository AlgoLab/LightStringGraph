#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <cstdlib>
#include <fstream>

#include "types.h"
#include "edge_joined_interval.h"
#include "joined_q_interval.h"

using std::ofstream;
using std::ifstream;

// convert nucleotide (enum) to char
char ntoc ( Nucleotide base );

// convert char to nucleotide
Nucleotide cton ( char c );

ofstream& operator<<( ofstream&, const EdgeInterval& );

ifstream& operator>>( ifstream&, EdgeInterval* );

ofstream& operator<<( ofstream&, const JoinedQInterval& );

ifstream& operator>>( ifstream&, JoinedQInterval* ); 

#endif
