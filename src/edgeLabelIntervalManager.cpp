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

#include "edgeLabelIntervalManager.h"

EdgeLabelIntervalManager::EdgeLabelIntervalManager( const vector< vector< string > >& filenamesIN )
  : _filenamesIN( filenamesIN )
{
  DEBUG_LOG_VERBOSE("EdgeLabelIntervalManager::Init");
  for( vector< vector< string > >::const_iterator it = _filenamesIN.begin();
       it != _filenamesIN.end(); ++it)
    {
      _edgeManagerVect.push_back( new SameLengthEdgeLabelIntervalManager( (*it) ) );
    }
  for( vector< SameLengthEdgeLabelIntervalManager* >::size_type i(0);
       i < _edgeManagerVect.size(); ++i )
    {
      if (_edgeManagerVect[i]->has_next_interval())
        {
          _pq.push(EdgeLabelEntry(_edgeManagerVect[i]->get_next_interval( ), i) );
        }
    }
}

EdgeLabelIntervalManager::~EdgeLabelIntervalManager( )
{
  while(!_pq.empty())
    {
      _pq.pop();
    }
  for( vector< SameLengthEdgeLabelIntervalManager* >::iterator it = _edgeManagerVect.begin();
       it != _edgeManagerVect.end(); ++it )
    {
      delete *it;
    }
}

SameLengthEdgeLabelIntervalManager& EdgeLabelIntervalManager::get_interval_manager( const EdgeLength& l )
{
  return *_edgeManagerVect[l];
}

void EdgeLabelIntervalManager::add_edge_interval( const EdgeLabelInterval& i, const EdgeLength& l, const Nucleotide& n )
{
  _edgeManagerVect[l]->add_interval( i, n );
}

void EdgeLabelIntervalManager::swap_files( )
{
  for( vector< SameLengthEdgeLabelIntervalManager* >::iterator it = _edgeManagerVect.begin();
       it != _edgeManagerVect.end(); ++it )
    (*it)->swap_files();

  while(!_pq.empty())
      _pq.pop();

  for( vector< SameLengthEdgeLabelIntervalManager* >::size_type i(0);
       i < _edgeManagerVect.size(); ++i )
    {
      if (_edgeManagerVect[i]->has_next_interval())
        {
          _pq.push(EdgeLabelEntry(_edgeManagerVect[i]->get_next_interval( ), i) );
        }
    }
}

bool EdgeLabelIntervalManager::get_next_interval( EdgeLabelEntry& e )
{
  if(_pq.empty())
    return false;

  // Pop the element
  e = _pq.top();
  _pq.pop();

  // Replace the element
  if (_edgeManagerVect[e._len]->has_next_interval()) {
    _pq.push(EdgeLabelEntry(_edgeManagerVect[e._len]->get_next_interval(), e._len));
  }
  return true;
}
