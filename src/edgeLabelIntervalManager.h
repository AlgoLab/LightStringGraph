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

#ifndef EDGELABELINTERVALMANAGER_H
#define EDGELABELINTERVALMANAGER_H

#include <vector>
#include <string>
#include <queue>

#include "types.h"
#include "interval_manager.h"
#include "edgeLabelInterval.h"

using std::vector;
using std::string;

struct EdgeLabelEntry
{
  EdgeLabelInterval _interval;
  EdgeLength _len;

  EdgeLabelEntry( )
    : _interval(), _len(0)
  {
  }

  EdgeLabelEntry( const EdgeLabelInterval& i, EdgeLength l)
    :_interval(i), _len(l)
  {
  }

  ~EdgeLabelEntry()
  {
  }

  EdgeLabelEntry& operator=(const EdgeLabelEntry& other)
  {
    if(this == &other) return *this;
    _interval = other._interval;
    _len = other._len;
    return *this;
  }

  bool operator<(const EdgeLabelEntry& rhs) const
  {
    return (_interval < rhs._interval);
  }
};

class EdgeLabelIntervalManager
{
public:
  typedef std::priority_queue< EdgeLabelEntry > EdgeLabel_PQ;

private:
  const vector< vector< string > > _filenamesIN;
  vector< SameLengthEdgeLabelIntervalManager* > _edgeManagerVect;
  EdgeLabel_PQ _pq;

public:
  // Constructor
  EdgeLabelIntervalManager( const vector< vector< string > >& );
  
  // Destructor
  ~EdgeLabelIntervalManager( );

  // Get Interval Manager for EdgeLength l and Nucleotide sigma
  SameLengthEdgeLabelIntervalManager& get_interval_manager( const EdgeLength& );

  // Add ArcInterval to specified interval manager
  void add_edge_interval( const EdgeLabelInterval&, const EdgeLength&, const Nucleotide& );

  // Get next interval, if any
  bool get_next_interval( struct EdgeLabelEntry& );

  // Swap all the intervalmanagers
  void swap_files( );

private:
};

#endif
