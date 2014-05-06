// -*- c++ -*-

#ifndef SEED_INTERVAL_H
#define SEED_INTERVAL_H

#include <cstddef>
#include "types.h"

class SeedInterval
{
public:
  // TODO: Complete this once we decide how to implement the seed intervals
  void add( SeedInterval& );
  void add( SequenceNumber );
  void clear();
  size_t size() const;
};

#endif
