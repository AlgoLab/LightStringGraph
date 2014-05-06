// -*- c++ -*-

#include "arcInterval.h"

ArcInterval::ArcInterval( const QInterval& q_in, const EdgeLength& edgeLen, const SeedInterval& seeds ):
  _interval( q_in.get_begin(), q_in.get_end() ), _len( edgeLen ), _seeds( seeds )
{

}


ArcInterval::~ArcInterval( )
{

}

const QInterval&  ArcInterval::get_q_interval( ) const
{
  return _interval;
}

const EdgeLength& ArcInterval::get_edge_length( ) const
{
  return _len;
}

const SeedInterval& ArcInterval::get_seeds( ) const
{
  return _seeds;
}

void ArcInterval::add_seed( SeedInterval& s )
{
  _seeds.add(s);
}
