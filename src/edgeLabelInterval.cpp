// -*- c++ -*-

#include "edgeLabelInterval.h"

EdgeLabelInterval::EdgeLabelInterval( const QInterval& forward, const QInterval& reverse )
  : _forward_interval( forward ), _reverse_interval( reverse )
{

}

EdgeLabelInterval::~EdgeLabelInterval( )
{

}

const QInterval& EdgeLabelInterval::get_label( ) const
{
  return _forward_interval;
}

const QInterval& EdgeLabelInterval::get_reverse_label( ) const
{
  return _reverse_interval;
}
