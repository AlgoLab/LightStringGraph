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

#ifndef LCPREADER_H
#define LCPREADER_H

#include <iostream>
#include <vector>
#include <string>

#include "types.h"
#include "partialLCPReader.h"

class LCPReader
{
private:
  std::vector< std::string >  _filenamesIN;
  unsigned short              _nextLCPIndex;
  partialLCPReader*           _currentLCP;
  LCPValue                    _lastLCP;


public:
  // Constructor
  LCPReader(std::vector< std::string >& filenamesIN);

  // Destructor
  ~LCPReader();

  // Return current position
  BWTPosition get_position() const;

  // Move to the next position if possibile.
  // Return value:
  //  - true if the position is reached
  //  - false otherwise
  bool next();

  // Get the current value of LCP
  LCPValue get() const;

private:
  // no need of copy ctor nor assignement operator
  LCPReader() { }
  LCPReader(const LCPReader& other) { }
  LCPReader& operator=(const LCPReader& other) { return *this; }
};

#endif
