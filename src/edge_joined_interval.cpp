#include "edge_joined_interval.h"

EdgeInterval::EdgeInterval( const BWTPosition& sbegin, const BWTPosition& send,
			    const BWTPosition& ebegin, const BWTPosition& eend )
  : _suffixI( sbegin, send ), _extendI( ebegin, eend ), _len( 1 )
{
  
}

EdgeInterval::EdgeInterval( const BWTPosition& sbegin, const BWTPosition& send,
			    const BWTPosition& ebegin, const BWTPosition& eend,
			    const EdgeLength& len )
  : _suffixI( sbegin, send ), _extendI( ebegin, eend ), _len( len )
{

}

EdgeInterval::~EdgeInterval( )
{
}

EdgeLength EdgeInterval::get_len( ) const
{
  return _len;
}

const QInterval& EdgeInterval::get_first_interval( ) const
{
  return _suffixI;
}

const QInterval& EdgeInterval::get_second_interval( ) const
{
  return _extendI;
}
