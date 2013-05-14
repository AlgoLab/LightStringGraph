#ifndef EDGE_JOINED_INTERVAL_H
#define EDGE_JOINED_INTERVAL_H

#include <vector>

#include "q_interval.h"
#include "types.h"

using std::vector;

/*******************************************************************************
 * EdgeInterval stores 2 intervals on BWT' and should be used during the right
 * extension procedure.
 *******************************************************************************/

class EdgeInterval
{
protected:
  QInterval _suffixI; // interval on BWT' relative to reads that have Q as suffix
  QInterval _extendI; // interval on BWT' relative to reads that have Q as
		      // prefix (the interval we should extend)
  EdgeLength _len; // Edge length

public:
  // ctor
  EdgeInterval( const BWTPosition &, const BWTPosition &,
		const BWTPosition &, const BWTPosition & );
  EdgeInterval( const BWTPosition &, const BWTPosition &,
		const BWTPosition &, const BWTPosition &, 
		const EdgeLength& );
  EdgeInterval( ) : _suffixI( 0, 0 ), _extendI( 0, 0 ), _len( 0 ) { };
  
  // destructor
  ~EdgeInterval( );

  EdgeLength get_len( ) const;
  const QInterval & get_first_interval( ) const;
  const QInterval & get_second_interval( ) const;

private:
  EdgeInterval( const EdgeInterval& other) : 
    _suffixI( 0, 0 ), _extendI( 0, 0 ), _len( 0 ) { };

  EdgeInterval& operator=( const EdgeInterval& other ) { return *this; }
};

#endif
