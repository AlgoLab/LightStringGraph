// -*- c++ -*-

#ifndef EDGELABELINTERVALMANAGER_H
#define EDGELABELINTERVALMANAGER_H

#include <vector>
#include <string>

#include "types.h"
#include "arcInterval.h"
#include "interval_manager.h"

using std::vector;
using std::string;

class EdgeLabelIntervalManager
{
private:
  vector< vector< string > > _filenamesIN;
  vector< SameLengthArcIntervalManager* > _edgeManagerVect;
  
public:
  // Constructor
  EdgeLabelIntervalManager( vector< vector< string > >& );
  
  // Destructor
  ~EdgeLabelIntervalManager( );

  // Get Interval Manager for EdgeLength l and Nucleotide sigma
  SameLengthArcIntervalManager& get_interval_manager( const EdgeLength& );

  // Add ArcInterval to specified interval manager
  void add_edge_interval( const ArcInterval&, const EdgeLength&, const Nucleotide& );

  // Swap all the intervalmanagers
  void swap_files( );

private:
};

#endif
