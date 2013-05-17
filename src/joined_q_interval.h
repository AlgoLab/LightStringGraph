#ifndef JOINED_Q_INTERVAL_H
#define JOINED_Q_INTERVAL_H

#include <vector>
#include <fstream>

#include "q_interval.h"

using std::vector;
using std::ofstream;
using std::ifstream;

/***********************************************************************/
/* JoinedQInterval represent a couple of q-intervals q1-int on BWT and */
/* q2-int on BWT such that q2 = rev(q1).			       */
/***********************************************************************/

class JoinedQInterval
{
 protected:
  QInterval _int, _revint; // int is on BWT while revint is on BWT'

 public:
  // ctor
  JoinedQInterval ( const BWTPosition & begin, const BWTPosition & end,
		    const BWTPosition & revbegin, const BWTPosition & revend );
  JoinedQInterval ( ) :  _int( 0, 0), _revint( 0, 0 ) { };
  // destructor
  ~JoinedQInterval ( );
  
  BWTPosition get_size ( ) const;
  const QInterval & get_interval ( ) const;
  const QInterval & get_reverse_interval ( ) const;

  friend ofstream& operator<<( ofstream&, const JoinedQInterval& );
  friend ifstream& operator>>( ifstream&, JoinedQInterval** );

  /* // Backward extend this interval with n and return the new */
  /* // JoinedQInterval for nQ and rev(Q)n */
  /* JoinedQInterval* backward_extend_with ( Nucleotide n ) const; */

  /* // Backward extend this interval with every possibile nucleotide in */
  /* // the alphabet */
  /* vector< JoinedQInterval* >* backward_extend_all ( ) const; */

 private:
  // no need of copy ctor nor assignment operator
  JoinedQInterval ( const JoinedQInterval& other ) :
  _int(0,0), _revint(0,0)
    { };
  JoinedQInterval& operator= ( const JoinedQInterval& other ) 
    { return *this; };
};

#endif
