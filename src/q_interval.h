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
class QInterval
{
private:
  BWTPosition   _begin,
    _end;
public:
  // Constructor
  QInterval ( const BWTPosition& begin,
              const BWTPosition& end)
      :_begin(begin), _end(end)
  {}

  // Copy ctor
  QInterval ( const QInterval& other )
      :_begin(other._begin), _end(other._end)
  {}

  // Destructor
  //~QInterval ( );

  // Get the size of the Q-interval
  // size equals to end - begin
  BWTPosition get_size ( ) const {
    return ( _end - _begin );
  }

  // Get the begin position of the Q-interval
  BWTPosition get_begin ( ) const {
    return _begin;
  }

  // Get the end position of the Q-interval
  BWTPosition get_end ( ) const {
    return _end;
  }


  // Equality operator
  bool operator==(const QInterval& rhs ) const {
    return (_begin == rhs._begin &&
            _end == rhs._end);
  }

  // Inequality operator
  bool operator!=(const QInterval& rhs) const {
    return !(*this==rhs);
  }

  // Assignment operator
  QInterval& operator= ( const QInterval& other ) {
    _begin = other._begin;
    _end = other._end;
    return *this;
  }

};

#endif
