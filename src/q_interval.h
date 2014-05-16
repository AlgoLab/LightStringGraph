// -*- c++ -*-

#ifndef Q_INTERVAL_H
#define Q_INTERVAL_H

#include "types.h"

/* Basic class that represent Q-Intervals. */
class QInterval
{
private:
  BWTPosition   _begin,
    _end;
public:
  // Constructor
  QInterval ( const BWTPosition& begin,
              const BWTPosition& end)
      :_begin(begin), _end(end)
  {}

  // Copy ctor
  QInterval ( const QInterval& other )
      :_begin(other._begin), _end(other._end)
  {}

  // Destructor
  //~QInterval ( );

  // Get the size of the Q-interval
  // size equals to end - begin
  BWTPosition get_size ( ) const {
    return ( _end - _begin );
  }

  // Get the begin position of the Q-interval
  BWTPosition get_begin ( ) const {
    return _begin;
  }

  // Get the end position of the Q-interval
  BWTPosition get_end ( ) const {
    return _end;
  }


  // Equality operator
  bool operator==(const QInterval& rhs ) const {
    return (_begin == rhs._begin &&
            _end == rhs._end);
  }

  // Inequality operator
  bool operator!=(const QInterval& rhs) const {
    return !(*this==rhs);
  }

  // Assignment operator
  QInterval& operator= ( const QInterval& other ) {
    _begin = other._begin;
    _end = other._end;
    return *this;
  }

};

#endif
