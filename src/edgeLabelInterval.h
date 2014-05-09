// -*- c++ -*-

#ifndef EDGELABELINTERVAL_H
#define EDGELABELINTERVAL_H

#include "q_interval.h"

class EdgeLabelInterval
{
private:
  QInterval _forward_interval, _reverse_interval;

public:
  // Constructor
  EdgeLabelInterval( const QInterval&, const QInterval& );

  // Destructor
  ~EdgeLabelInterval( );

  EdgeLabelInterval( const EdgeLabelInterval& other )
    : _forward_interval(other.get_label()), _reverse_interval(other.get_reverse_label())
  { };

  // get label
  const QInterval& get_label( ) const;
  const QInterval& get_reverse_label( ) const;

  // Equality operator
  bool operator==(const EdgeLabelInterval& rhs ) const;

  // Inequality operator
  bool operator!=(const EdgeLabelInterval& rhs) const;

  // Assignement operator
  EdgeLabelInterval& operator= ( const EdgeLabelInterval& other );

  bool operator>(const EdgeLabelInterval& rhs) const;
  bool operator<(const EdgeLabelInterval& rhs) const;

private:
  // No need of copu ctor nor assigment operator
  EdgeLabelInterval( ) : _forward_interval(0,0), _reverse_interval(0, 0) { };
};

#endif
