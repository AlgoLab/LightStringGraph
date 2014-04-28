// -*- c++ -*-
#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <sstream>
#include <ctime>

#include "types.h"
#include "edge_joined_interval.h"
#include "joined_q_interval.h"
#include "q_interval.h"

using std::ofstream;
using std::ifstream;
using std::vector;

// convert nucleotide (enum) to char
char ntoc ( Nucleotide base );

// convert char to nucleotide
Nucleotide cton ( char c );

// Legacy
// Check if there is some irreducible edge between the reads pointed
// by two Q-intervals
// void checkIfIrreducible( SGraph&, Precedencies&, GSAEntry*, GSAEntry*, EdgeLength& );

// Overloading various operato<< and operator>> ..
ofstream& operator<<( ofstream&, const QInterval& );
ifstream& operator>>( ifstream&, QInterval** );

// ofstream& operator<<( ofstream&, const EdgeInterval& );
// ifstream& operator>>( ifstream&, EdgeInterval* );
// ifstream& operator>>( ifstream&, EdgeInterval** );

// ofstream& operator<<( ofstream&, const JoinedQInterval& );
// ifstream& operator>>( ifstream&, JoinedQInterval* );

ofstream& operator<<( ofstream&, const GSAEntry& );
ifstream& operator>>( ifstream&, GSAEntry& );

// Return a string representing the current tim
std::string now( const char* format );

#ifdef DEBUG
#define DEBUG_LOG(s)                                        \
  std::cerr << "DBG:" << __FILE__ << "(" << __LINE__ << ")" \
  << " " << s << std::endl;
#else
#define DEBUG_LOG(s) ;
#endif

#ifdef DEBUG_VERBOSE
#define DEBUG_LOG_VERBOSE(s)                                \
  DEBUG_LOG(s)
#else
#define DEBUG_LOG_VERBOSE(s) ;
#endif

#endif
