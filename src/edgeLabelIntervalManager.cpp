// -*- c++ -*-

#include "edgeLabelIntervalManager.h"

EdgeLabelIntervalManager::EdgeLabelIntervalManager( vector< vector< string > >& filenamesIN )
  : _filenamesIN( filenamesIN )
{
  DEBUG_LOG_VERBOSE("EdgeLabelIntervalManager::Init");
  for( vector< vector< string > >::iterator it = _filenamesIN.begin();
       it != _filenamesIN.end(); ++it)
    {
      _edgeManagerVect.push_back( new SameLengthArcIntervalManager( (*it) ) );
    }
}

EdgeLabelIntervalManager::~EdgeLabelIntervalManager( )
{
  for( vector< SameLengthArcIntervalManager* >::iterator it = _edgeManagerVect.begin();
       it != _edgeManagerVect.end(); ++it )
    {
      delete *it;
    }
}

SameLengthArcIntervalManager& EdgeLabelIntervalManager::get_interval_manager( const EdgeLength& l )
{
  return *_edgeManagerVect[l];
}

void EdgeLabelIntervalManager::add_edge_interval( const ArcInterval& i, const EdgeLength& l, const Nucleotide& n )
{
  _edgeManagerVect[l]->add_interval( i, n );
}

void EdgeLabelIntervalManager::swap_files( )
{
  for( vector< SameLengthArcIntervalManager* >::iterator it = _edgeManagerVect.begin();
       it != _edgeManagerVect.end(); ++it )
    (*it)->swap_files();
}