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

#ifndef EDGELABELINTERVAL_H
#define EDGELABELINTERVAL_H

#include "q_interval.h"

struct EdgeLabelInterval
{
  QInterval label, reverse_label;

  // Constructors
  EdgeLabelInterval( )
    :label(), reverse_label()
  { };
  EdgeLabelInterval( const QInterval& forward, const QInterval& reverse )
    : label( forward ), reverse_label( reverse )
  { };

  // Destructor
  ~EdgeLabelInterval( )
  { };

  EdgeLabelInterval( const EdgeLabelInterval& other )
    : label(other.label),
      reverse_label(other.reverse_label)
  { };

  // Equality operator
  bool operator==(const EdgeLabelInterval& rhs ) const {
    return ((label == rhs.label) &&
            (reverse_label == rhs.reverse_label));
  }

  // Inequality operator
  bool operator!=(const EdgeLabelInterval& rhs) const {
    return (!((*this) == rhs));
  }

  // Assignement operator
  EdgeLabelInterval& operator= ( const EdgeLabelInterval& other ) {
    label = other.label;
    reverse_label = other.reverse_label;
    return *this;
  }

  bool operator>(const EdgeLabelInterval& rhs) const {
    return (label.begin < rhs.label.begin ||
            (label.begin == rhs.label.begin &&
             label.end >= rhs.label.end));
  }

  bool operator<(const EdgeLabelInterval& rhs) const {
    return rhs > *this;
  }

};

#endif
