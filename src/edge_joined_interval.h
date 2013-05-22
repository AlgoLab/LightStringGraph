#ifndef EDGE_JOINED_INTERVAL_H
#define EDGE_JOINED_INTERVAL_H

#include <vector>
#include <fstream>
#include <iostream>

#include "q_interval.h"
#include "types.h"

using std::vector;
using std::ofstream;
using std::ifstream;

/*******************************************************************************
 * EdgeInterval stores 2 intervals on BWT' and should be used during the right
 * extension procedure.
 *******************************************************************************/

class EdgeInterval
{
protected:
  vector< QInterval* > _suffixI; // intervals on BWT' relative to reads that
				 // have Q as suffix
  QInterval _extendI; // interval on BWT' relative to reads that have Q as
		      // prefix (the interval we should extend)
  vector< EdgeLength > _len; // Edge length

public:
  // ctor
  EdgeInterval( const BWTPosition &, const BWTPosition &,
		const BWTPosition &, const BWTPosition & );
  EdgeInterval( const BWTPosition &, const BWTPosition &,
		const BWTPosition &, const BWTPosition &, 
		const EdgeLength& );
  EdgeInterval( ) : _suffixI( ), _extendI( 0, 0 ), _len( 0 ) { };
  EdgeInterval( const BWTPosition&, const BWTPosition&, vector< QInterval* >&,
		vector< EdgeLength >& );

  // destructor
  ~EdgeInterval( );

  friend ofstream& operator<<( ofstream&, const EdgeInterval& );
  friend ifstream& operator>>( ifstream&, EdgeInterval** );

  void add_suffix_interval( const QInterval*, const EdgeLength );

  vector< EdgeLength >& get_len( );
  const QInterval & get_first_interval( ) const;
  vector< QInterval* >& get_second_interval( );

private:
  EdgeInterval( const EdgeInterval& other) : 
  _suffixI( ), _extendI( 0, 0 ), _len( 0 ) { };

  EdgeInterval& operator=( const EdgeInterval& other ) { return *this; }
};

#endif
