// -*- c++ -*-

#include "LCPIterator.h"

#include "util.h"

std::istream& operator>> (std::istream& is, LCPFileValue& li) {
  is.read(reinterpret_cast<char*>(&li.v), sizeof(LCPFileValue));
  return is;
}
