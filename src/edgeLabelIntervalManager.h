// -*- c++ -*-

#ifndef EDGELABELINTERVALMANAGER_H
#define EDGELABELINTERVALMANAGER_H

#include <vector>
#include <string>

#include "types.h"
#include "interval_manager.h"
#include "edgeLabelInterval.h"

using std::vector;
using std::string;

class EdgeLabelIntervalManager
{
private:
  vector< vector< string > > _filenamesIN;
  vector< SameLengthEdgeLabelIntervalManager* > _edgeManagerVect;
  
public:
  // Constructor
  EdgeLabelIntervalManager( vector< vector< string > >& );
  
  // Destructor
  ~EdgeLabelIntervalManager( );

  // Get Interval Manager for EdgeLength l and Nucleotide sigma
  SameLengthEdgeLabelIntervalManager& get_interval_manager( const EdgeLength& );

  // Add ArcInterval to specified interval manager
  void add_edge_interval( const EdgeLabelInterval&, const EdgeLength&, const Nucleotide& );

  // Swap all the intervalmanagers
  void swap_files( );

private:
};

#endif
