// -*- c++ -*-
#ifndef LCPITERATOR_H
#define LCPITERATOR_H

#include <istream>

#include "types.h"
#include "MultiFileIterator.h"

struct LCPFileValue {
  LCPValue v;
};

std::istream& operator>> (std::istream& is, LCPFileValue& li);

template <>
struct get_value<LCPFileValue,LCPValue> {
  LCPValue operator()(const LCPFileValue lfv) {
	 return LCPValue(lfv.v);
  }
};


typedef MultiFileIterator<LCPFileValue, LCPValue> LCPIterator;

#endif
