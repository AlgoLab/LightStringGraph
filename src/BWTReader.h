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

#ifndef BWTREADER_H
#define BWTREADER_H

#include <vector>
#include <string>
#include <cstdlib>

#include "types.h"

#include "BWTIterator.h"

class BWTReader
{
private:
  BWTIterator _bwt_it;
  BWTPosition _maxPosition;

  std::vector< NucleoCounter > _current_pi;

public:
  // Constructor
  BWTReader ( const std::vector< std::string >& filenamesIN )
    : _bwt_it(filenamesIN), _maxPosition(0), _current_pi(ALPHABET_SIZE, 0)
  {
  }

  // Destructor
  ~BWTReader ( ) {
  }

  // Returns current position
  BWTPosition get_position ( ) const {
    return _bwt_it.get_position();
  }

  // Returns max position
  BWTPosition size( ) const {
    return _maxPosition;
  }

  // Move to position p in this BWT and update nucleotide occurrences accordingly.
  // Return value:
  // - true if p can be reached
  // - false otherwise
  bool move_to ( const BWTPosition& p ) {
    while (!_bwt_it.terminated() && (_bwt_it.get_position() < p)) {
      const Nucleotide _current_nuc= NuclConv::cton(*_bwt_it);
      ++_current_pi[_current_nuc];
      ++_bwt_it;
    }
    return _bwt_it.get_position() == p;
  }

  // Return PI vector (occurrences of every nucleotide before current position).
  const std::vector< NucleoCounter >& get_Pi ( ) const {
    return _current_pi;
  }

  // Return C (count)
  std::vector< NucleoCounter > get_C ( ) {
    reset();
    while (!_bwt_it.terminated()) {
      const Nucleotide _current_nuc= NuclConv::cton(*_bwt_it);
      ++_current_pi[_current_nuc];
      ++_bwt_it;
    }
    std::vector< NucleoCounter > C(ALPHABET_SIZE, 0);
    for (size_t i= 1; i<ALPHABET_SIZE; ++i) {
      C[i] = C[i-1] + _current_pi[i-1];
    }
    _maxPosition= _bwt_it.get_position();
    reset();
    return C;
  }

  // Reset BWT
  void reset ( ) {
    for (size_t i= 0; i<ALPHABET_SIZE; ++i) {
      _current_pi[i]= 0;
    }
    _bwt_it.reset();
  }

  // Get current nucleotide
  Nucleotide get_current_nucleotide() {
    return NuclConv::cton(*_bwt_it);
  }

private:
  // no need of copy ctor nor assignment operator
  BWTReader ( );
  BWTReader ( const BWTReader& );
  BWTReader& operator= ( const BWTReader& );

};

#endif
