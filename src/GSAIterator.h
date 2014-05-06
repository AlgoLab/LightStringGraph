// -*- c++ -*-
#ifndef GSA_ITERATOR_H
#define GSA_ITERATOR_H

#include <fstream>
#include <vector>
#include <string>
#include <iterator>

#include "types.h"
#include "SingleFileIterator.h"

std::istream& operator>> (std::istream& is, GSAEntry& li);

typedef SingleFileIterator<GSAEntry> GSAIterator;

#endif
