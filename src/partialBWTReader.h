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

#ifndef PARTIAL_BWTREADER_H
#define PARTIAL_BWTREADER_H

#include <vector>
#include <string>
#include <cstdlib>

#ifdef HAS_ZLIB
#include <zlib.h>
#else
#include <cstdio>
#endif

#include "types.h"
#include "util.h"


using std::vector;
using std::string;

// Class that handles a single portion of BWT.

class partialBWTReader
{
private:
#ifdef HAS_ZLIB
  gzFile         _fileIn;         // File from which we want to read;
#else
  FILE*          _fileIn;         // File from which we want to read;
#endif
  char* const    _buffer;         // Current buffer
  BWTPosition    _start;          // Buffer start position in BWT
  BWTPosition    _position;       // Position reached within the buffer
  BWTPosition    _bufferlen;      // Number of char read in the last "read"
                                  // call, used while moving inside the buffer
  vector< NucleoCounter >   _occurrencesBeforeStart;  // PI vector

public:
  // Constructor
  partialBWTReader ( const string& inputFilename,
                     BWTPosition start= 0,
                     const vector< NucleoCounter >& occurrencesBeforeStart= vector< NucleoCounter >(ALPHABET_SIZE, 0));

  // Destructor
  ~partialBWTReader ( ) {
    delete[] _buffer;
    if (_fileIn != NULL) {
#ifdef HAS_ZLIB
      gzclose(_fileIn);
#else
      fclose(_fileIn);
#endif
    }
  }

  // Get occurrences before current position
  const vector< NucleoCounter >& get_Pi ( ) const {
    return _occurrencesBeforeStart;
  }

  // Get absolute position in BWT
  BWTPosition get_position ( ) const {
    return (_start + _position);
  }

  // Move to position p in this BWT and update nucleotide occurrences accordingly.
  // Return value:
  // - true if p can be reached
  // - false otherwise
  bool move_to ( const BWTPosition & p );

  // Return nucleotide in the current position
  char get_current_nucleotide_char( ) const {
    return _buffer[_position];
  }

};

#endif
