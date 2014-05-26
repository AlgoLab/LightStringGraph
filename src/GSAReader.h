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
 * along with Nome-Programma.  If not, see <http://www.gnu.org/licenses/>.
 *
 **/

#ifndef GSA_READER_H
#define GSA_READER_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

#include "types.h"
#include "util.h"

using std::vector;
using std::string;

class GSAReader
{
private:
  std::ifstream   _inputFile;
  BWTPosition     _currentPosition;
  BWTPosition     _maxPosition;
  GSAEntry			_lastGSA;					// Last read GSAEntry

public:
  // Constructor
  GSAReader( const string& );

  // Destructor
  ~GSAReader( );

  // Move GSAReader to a given position
  // Return true if position is reached, false otherwise
  bool move_to( const BWTPosition& );

  // Reset GSAReader
  void reset( );

  // Get last read GSAEntry
  GSAEntry get_v( );

  // Gets reads index of contiguous suffixes which length is "length" starting
  // from _currentPosition up to end.
  // _currentPosition and _inputFile will be modified!
  void get_seq_sent( int length, SeqNExtVect& rsuff, BWTPosition end );

  // Gets positions of BASE_$ occurrences in BWT from _currentPosition to position
  // Positions that exceed end will not be stored.
  void get_seq_at_pos( BWTPExtVect& position, SeqNExtVect& indexes, BWTPosition end);

  // Get _currentPosition
  BWTPosition get_position( );

private:
  GSAReader( ) { };
  GSAReader( const GSAReader& other ) { };
  GSAReader& operator=( const GSAReader& other ) { return *this; };

};

#endif
