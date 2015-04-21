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

#ifndef Q_INTERVAL_H
#define Q_INTERVAL_H

#include "types.h"

/* Basic class that represent Q-Intervals. */
template <typename Type>
struct base_interval
{
  Type begin,
    end;

  // Constructor
  base_interval ( const Type& begin_,
              const Type& end_)
      :begin(begin_), end(end_)
  {}

  // Copy ctor
  base_interval ( const base_interval<Type>& other )
      :begin(other.begin), end(other.end)
  {}

  // Destructor
  //~base_interval ( );

  // Get the size of the Q-interval
  // size equals to end - begin
  Type size ( ) const {
    return ( end - begin );
  }

  // Equality operator
  bool operator==(const base_interval<Type>& rhs ) const {
    return (this->begin == rhs.begin && this->end == rhs.end);
  }

  // Inequality operator
  bool operator!=(const base_interval<Type>& rhs) const {
    return !(*this==rhs);
  }

  // Assignment operator
  base_interval<Type>& operator= ( const base_interval<Type>& other ) {
    begin = other.begin;
    end = other.end;
    return *this;
  }

};

typedef base_interval<BWTPosition> QInterval;

#endif
