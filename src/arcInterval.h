// -*- c++ -*-

#ifndef ARC_INTERVAL_H
#define ARC_INTERVAL_H

#include "q_interval.h"
#include "types.h"
#include "seedInterval.h"

class ArcInterval
{
private:
  QInterval     _interval;
  EdgeLength    _len;
  // TODO: Add something here
  SeedInterval _seeds;

public:
  // Constructor
  ArcInterval( const QInterval&, const EdgeLength&, const SeedInterval& seeds);

  // Destructor
  ~ArcInterval( );

  // Get the QInterval
  const QInterval& get_q_interval( ) const;

  // Get the length of the linked LabelInterval
  const EdgeLength& get_edge_length( ) const;

  // Get the Seeds
  const SeedInterval& get_seeds( ) const;

  void add_seed(SeedInterval&);

private:
  ArcInterval( ) : _interval(0,0) { };
  ArcInterval( const ArcInterval& other ) : _interval(0,0) { };
  ArcInterval& operator=( const ArcInterval& other )
  { return *this; };
};

#endif
