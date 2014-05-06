// -*- c++ -*-
#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <sstream>
#include <ctime>
#include <stdexcept>

#include "types.h"
#include "edge_joined_interval.h"
#include "joined_q_interval.h"
#include "q_interval.h"
#include "arcInterval.h"

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
ifstream& operator>>( ifstream&, QInterval* );

// ofstream& operator<<( ofstream&, const EdgeInterval& );
// ifstream& operator>>( ifstream&, EdgeInterval* );
// ifstream& operator>>( ifstream&, EdgeInterval** );

// ofstream& operator<<( ofstream&, const JoinedQInterval& );
// ifstream& operator>>( ifstream&, JoinedQInterval* );

ofstream& operator<<( ofstream&, const GSAEntry& );
ifstream& operator>>( ifstream&, GSAEntry& );

ofstream& operator<<( ofstream&, const ArcInterval& );
ifstream& operator>>( ifstream&, ArcInterval* );
//ifstream& operator>>( ifstream&, ArcInterval** );

ofstream& operator<<( ofstream&, const SeedInterval& );
ifstream& operator>>( ifstream&, SeedInterval& );

// Return a string representing the current tim
std::string now( const char* format );

#ifdef DEBUG
#define DEBUG_LOG(s)                          \
  do {                                        \
    std::cerr << "DBG:"                       \
              << __FILE__ << ":" << __LINE__  \
              << "  " << s << std::endl;      \
  } while (0)
#else
#define DEBUG_LOG(s) do {} while(0)
#endif

#ifdef DEBUG_VERBOSE
#define DEBUG_LOG_VERBOSE(s) DEBUG_LOG(s)
#else
#define DEBUG_LOG_VERBOSE(s) do {} while(0)
#endif



#define QUOTE(str) #str
#define EXPAND_AND_QUOTE(str) QUOTE(str)

#define _MY_FAIL                               \
  do {                                         \
    DEBUG_LOG("Failure.");                     \
    throw std::logic_error("Failure at " +     \
                           std::string(__FILE__) + ":" +              \
                           std::string(EXPAND_AND_QUOTE(__LINE__)) ); \
  } while (0)

#define _FAIL_AND_LOG( _MESSAGE_ )     \
  do {                                 \
    DEBUG_LOG( _MESSAGE_ );            \
    throw std::logic_error(_MESSAGE_); \
  } while (0)

#define _FAIL_IF( _TEST_ )                    \
  do {                                        \
    if (_TEST_) {                             \
      DEBUG_LOG("Condition '" <<              \
                EXPAND_AND_QUOTE(_TEST_) <<   \
                "' verified.");               \
      _MY_FAIL;                               \
    }                                         \
  } while (0)



#ifdef DEBUG  // DEBUG is ON

#define PERFORM_AND_CHECK( TEST )                    \
  do {                                               \
    bool __test_perform_##__LINE__ = TEST;           \
      if (!__test_perform_##__LINE__) {              \
        DEBUG_LOG("Error while performing '" <<      \
                  EXPAND_AND_QUOTE(TEST) << "'.");   \
        _MY_FAIL;                                    \
      }                                              \
  } while(0)

#else // DEBUG is OFF

#define PERFORM_AND_CHECK( TEST )  \
  do {                             \
    TEST;                          \
  } while(0)

#endif

#endif
