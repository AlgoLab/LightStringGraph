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
#include <climits>

#include "types.h"
#include "edge_joined_interval.h"
#include "joined_q_interval.h"
#include "q_interval.h"
#include "arcInterval.h"
#include "edgeLabelInterval.h"

using std::ofstream;
using std::ifstream;
using std::vector;

#define LSG_INFIX_TMP ".tmplsg"
#define LSG_INFIX_OUT ".outlsg"

// convert nucleotide (enum) <-> char
// ntoc: enum -> char
// cton: char -> enum
// WARNING: Does not perform safety checks!!
class NuclConv {
public:
  static Nucleotide cton(const char c) {
	 return _conv._cton[(unsigned int)c];
  }

  static char ntoc(const Nucleotide n) {
	 return _conv._ntoc[n];
  }

private:
  NuclConv();
  Nucleotide _cton[UCHAR_MAX+1];
  char _ntoc[ALPHABET_SIZE];
  const static NuclConv _conv;
};

template <typename T>
std::string convert_to_string(const T& el) {
  std::stringstream stream;
  stream << el;
  return stream.str();
}

// Legacy
// Check if there is some irreducible edge between the reads pointed
// by two Q-intervals
// void checkIfIrreducible( SGraph&, Precedencies&, GSAEntry*, GSAEntry*, EdgeLength& );

// Overloading various operato<< and operator>> ..
ofstream& operator<<( ofstream&, const QInterval& );
ifstream& operator>>( ifstream&, QInterval*& );

// ofstream& operator<<( ofstream&, const EdgeInterval& );
// ifstream& operator>>( ifstream&, EdgeInterval* );
// ifstream& operator>>( ifstream&, EdgeInterval** );

// ofstream& operator<<( ofstream&, const JoinedQInterval& );
// ifstream& operator>>( ifstream&, JoinedQInterval* );

ofstream& operator<<( ofstream&, const GSAEntry& );
ifstream& operator>>( ifstream&, GSAEntry& );

ofstream& operator<<( ofstream&, const ArcInterval& );
ifstream& operator>>( ifstream&, ArcInterval*& );

ofstream& operator<<( ofstream&, const SeedInterval& );
ifstream& operator>>( ifstream&, SeedInterval*& );

ofstream& operator<<( ofstream&, const EdgeLabelInterval& );
ifstream& operator>>( ifstream&, EdgeLabelInterval*& );

// Return a string representing the current tim
std::string now( const char* format );

#ifdef DEBUG
#define DEBUG_LOG(s)                                                    \
  do {                                                                  \
    std::cerr << "DBG:"                                                 \
              << __FILE__ << ":"                                        \
              << std::setiosflags (std::ios::left) << std::setw(4)      \
              << __LINE__                                               \
              << "  " << s << std::endl;                                \
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
