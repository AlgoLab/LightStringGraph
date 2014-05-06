// -*- c++ -*-

#include "util.h"
#include "GSAIterator.h"

std::istream& operator>> (std::istream& is, GSAEntry& li) {
  is.read(reinterpret_cast<char*>(&li), sizeof(GSAEntry));
  return is;
}

