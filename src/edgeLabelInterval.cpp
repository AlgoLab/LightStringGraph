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

bool EdgeLabelInterval::operator==(const EdgeLabelInterval& rhs) const
{
  return ((_forward_interval == rhs.get_label()) &&
          (_reverse_interval == rhs.get_reverse_label()));
}

bool EdgeLabelInterval::operator!=(const EdgeLabelInterval& rhs) const
{
  return (!((*this) == rhs));
}

EdgeLabelInterval& EdgeLabelInterval::operator=( const EdgeLabelInterval& other)
{
  _forward_interval = other.get_label();
  _reverse_interval = other.get_reverse_label();
  return *this;
}

bool EdgeLabelInterval::operator>( const EdgeLabelInterval& rhs) const
{
  if((_forward_interval.get_end() > rhs.get_label().get_end()) ||
     ((_forward_interval.get_end() == rhs.get_label().get_end()) &&
      (_forward_interval.get_begin() < rhs.get_label().get_begin())))
    return true;
  else
    return false;
}

bool EdgeLabelInterval::operator<( const EdgeLabelInterval& rhs) const
{
  return rhs > *this;
}
