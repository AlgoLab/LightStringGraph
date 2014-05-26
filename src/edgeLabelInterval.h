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

class EdgeLabelInterval
{
private:
  QInterval _forward_interval, _reverse_interval;

public:
  // Constructor
  EdgeLabelInterval( const QInterval&, const QInterval& );

  // Destructor
  ~EdgeLabelInterval( );

  EdgeLabelInterval( const EdgeLabelInterval& other )
    : _forward_interval(other.get_label()), _reverse_interval(other.get_reverse_label())
  { };

  // get label
  const QInterval& get_label( ) const;
  const QInterval& get_reverse_label( ) const;

  // Equality operator
  bool operator==(const EdgeLabelInterval& rhs ) const;

  // Inequality operator
  bool operator!=(const EdgeLabelInterval& rhs) const;

  // Assignement operator
  EdgeLabelInterval& operator= ( const EdgeLabelInterval& other );

  bool operator>(const EdgeLabelInterval& rhs) const;
  bool operator<(const EdgeLabelInterval& rhs) const;

private:
  // No need of copu ctor nor assigment operator
  EdgeLabelInterval( ) : _forward_interval(0,0), _reverse_interval(0, 0) { };
};

#endif
