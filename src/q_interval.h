// -*- c++ -*-

#ifndef Q_INTERVAL_H
#define Q_INTERVAL_H

#include <types.h>

/* Basic class that represent Q-Intervals. */

struct PODQInterval
{
  BWTPosition   _begin,
    _end;
};

class QInterval : public PODQInterval
{
public:
  // Constructor
  QInterval ( const BWTPosition&, const BWTPosition& );

  // Copy ctor
  QInterval ( const QInterval& other );

  // Destructor
  ~QInterval ( );

  // Get the size of the Q-interval
  // size equals to end - begin
  BWTPosition get_size ( ) const;

  // Get the begin position of the Q-interval
  BWTPosition get_begin ( ) const;

  // Get the end position of the Q-interval
  BWTPosition get_end ( ) const;

  // Equality operator
  bool operator==(const QInterval& rhs ) const;

  // Inequality operator
  bool operator!=(const QInterval& rhs) const;

  // Assignement operator
  QInterval& operator= ( const QInterval& other );

private:
  // no need of copy ctor nor assignment operator
  QInterval ( ) { };
};

#endif
