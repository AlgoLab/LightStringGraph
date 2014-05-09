// -*- c++ -*-

#include "edgeLabelIntervalManager.h"

EdgeLabelIntervalManager::EdgeLabelIntervalManager( vector< vector< string > >& filenamesIN )
  : _filenamesIN( filenamesIN )
{
  DEBUG_LOG_VERBOSE("EdgeLabelIntervalManager::Init");
  for( vector< vector< string > >::iterator it = _filenamesIN.begin();
       it != _filenamesIN.end(); ++it)
    {
      _edgeManagerVect.push_back( new SameLengthEdgeLabelIntervalManager( (*it) ) );
    }
  for( vector< SameLengthEdgeLabelIntervalManager* >::size_type i(0); i < _edgeManagerVect.size(); ++i )
    {
      EdgeLabelInterval* x = _edgeManagerVect[i]->get_next_interval( );
      if( x != NULL )
        {
          EdgeLabelEntry elbe(x, i);
          _pq.push( elbe );
          delete elbe._interval;
          x = NULL;
        }
    }
}

EdgeLabelIntervalManager::~EdgeLabelIntervalManager( )
{
  while(!_pq.empty())
    {
      delete _pq.top()._interval;
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
    {
      _pq.pop();
    }
  
  for( vector< SameLengthEdgeLabelIntervalManager* >::size_type i(0); i < _edgeManagerVect.size(); ++i )
    {
      EdgeLabelInterval* x = _edgeManagerVect[i]->get_next_interval( );
      if( x != NULL )
        {
          EdgeLabelEntry elbe(x, i);
          _pq.push( elbe );
          delete elbe._interval;
        }
    }
}

bool EdgeLabelIntervalManager::get_next_interval( EdgeLabelEntry& e )
{
  if(_pq.empty())
    return false;

  // Pop the element
  e = _pq.top();
  delete _pq.top()._interval;
  _pq.pop();

  // Replace the element
  EdgeLabelInterval* ei_replacement = _edgeManagerVect[e._len]->get_next_interval();
  if( ei_replacement != NULL )
    {
      EdgeLabelEntry elbe(ei_replacement, e._len);
      _pq.push(elbe);
      delete elbe._interval;
      ei_replacement = NULL;
    }
  return true;
}
