#include <joined_q_interval.h>

JoinedQInterval::JoinedQInterval ( const BWTPosition & begin, const BWTPosition & end,
				   const BWTPosition & revbegin, const BWTPosition & revend ) 
  : _int ( begin, end ), _revint ( revbegin, revend )
{
}

BWTPosition JoinedQInterval::get_size ( ) const
{
  return _int.get_size();
}

const QInterval & JoinedQInterval::get_interval ( ) const
{
  return _int;
}

const QInterval & JoinedQInterval::get_reverse_interval ( ) const
{
  return _revint;
}
