/*
 * Basic class that represent Q-Intervals.
 */

#include <types.h>

#ifndef Q_INTERVAL_H
#define Q_INTERVAL_H

class QInterval
{
 protected:
  BWTPosition _begin, _end; // interval's start and end

 public:
  QInterval ( const BWTPosition&, const BWTPosition& );
  ~QInterval ( );
  BWTPosition get_size ( ) const;
  BWTPosition get_begin ( ) const;
  BWTPosition get_end ( ) const;

 private:
  // no need of copy ctor nor assignment operator
  QInterval ( ) { };
  QInterval ( const QInterval& other ) 
    { };
  QInterval& operator= ( const QInterval& other ) 
    { return *this; };
};

#endif
