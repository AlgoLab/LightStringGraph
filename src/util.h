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
#include <cstdio>

#include <zlib.h>

#include <boost/lexical_cast.hpp>

#include "types.h"
#include "q_interval.h"
#include "arcInterval.h"
#include "edgeLabelInterval.h"
#include "file_abstractions.h"
#include "util_log.h"

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
  return boost::lexical_cast<std::string>(el);
}

// Overloading various writing and reading functions
void write_interval( gzFile , const QInterval& );
bool read_interval ( gzFile , QInterval& );

ofstream& operator<<( ofstream&, const GSAEntry& );
ifstream& operator>>( ifstream&, GSAEntry& );

void write_interval( gzFile , const ArcInterval& );
bool read_interval ( gzFile , ArcInterval& );

void write_interval( gzFile , const SeedInterval& );
bool read_interval ( gzFile , SeedInterval& );

void write_interval( gzFile , const EdgeLabelInterval& );
bool read_interval ( gzFile , EdgeLabelInterval& );

// Return a string representing the current tim
std::string now( const char* format );


#endif
