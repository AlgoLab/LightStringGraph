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

  // get label
  const QInterval& get_label( ) const;
  const QInterval& get_reverse_label( ) const;

private:
  // No need of copu ctor nor assigment operator
  EdgeLabelInterval( ) : _forward_interval(0,0), _reverse_interval(0, 0) { };
  EdgeLabelInterval( const EdgeLabelInterval& other ) : _forward_interval(0,0), _reverse_interval(0,0) { };
  EdgeLabelInterval& operator=( const EdgeLabelInterval& other ) { return *this; };
};

#endif
