// -*- c++ -*-

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

struct EdgeLabelEntry;

struct EdgeLabelEntry
{
  EdgeLabelInterval* _interval;
  EdgeLength _len;

  EdgeLabelEntry( )
  {
    _interval = NULL;
    _len =0;
  }

  EdgeLabelEntry( EdgeLabelInterval* i, EdgeLength l)
  {
    _interval = new EdgeLabelInterval(*(i));
    _len = l;
  }

  ~EdgeLabelEntry()
  {
    _interval=NULL;
  }

  struct EdgeLabelEntry& operator=(const struct EdgeLabelEntry& other)
  {
    _interval = new EdgeLabelInterval(*(other._interval));
    _len = other._len;
    return *this;
  }

  bool operator<(const struct EdgeLabelEntry& rhs) const
  {
    // Use > instead of < to hack prority queue
    return (*_interval > *(rhs._interval));
  }

};

class EdgeLabelIntervalManager
{
public:
  typedef std::priority_queue< struct EdgeLabelEntry > EdgeLabel_PQ;

private:
  vector< vector< string > > _filenamesIN;
  vector< SameLengthEdgeLabelIntervalManager* > _edgeManagerVect;
  EdgeLabel_PQ _pq;

public:
  // Constructor
  EdgeLabelIntervalManager( vector< vector< string > >& );
  
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
