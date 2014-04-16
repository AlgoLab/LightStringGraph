#include <q_interval.h>

QInterval::QInterval ( const BWTPosition& begin,
                       const BWTPosition& end )
{
  _begin = begin;
  _end = end;
}

QInterval::~QInterval ( )
{
}

BWTPosition QInterval::get_size ( ) const
{
  return ( _begin - _end );
}

BWTPosition QInterval::get_begin ( ) const
{
  return _begin;
}

BWTPosition QInterval::get_end ( ) const
{
  return _end;
}
