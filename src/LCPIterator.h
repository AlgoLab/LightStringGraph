// -*- c++ -*-
#ifndef LCPITERATOR_H
#define LCPITERATOR_H

#include <iostream>
#include <vector>
#include <string>
#include <iterator>

#include "types.h"

struct LCPFileValue {
  LCPValue v;
};

std::istream& operator>> (std::istream& is, LCPFileValue& li);

class LCPIterator
  : public std::iterator< std::input_iterator_tag,
								  LCPValue >
{
private:
  typedef std::istream_iterator<LCPFileValue> lcp_iterator_t;
  typedef std::vector< lcp_iterator_t > lcp_iterators_t;

  const std::istream_iterator<LCPFileValue> _eoi;
  std::vector<std::ifstream*>  _files;
  lcp_iterators_t  _iterators;
  lcp_iterators_t::iterator _current_iterator;

  BWTPosition _current_position;
  bool _terminated;


public:
  // Constructors
  LCPIterator(const std::vector< std::string >& filenames);

  // Destructor
  ~LCPIterator();

  LCPIterator& operator++();
  LCPValue operator*();

  BWTPosition get_position() const {
	 return _current_position;
  }

  bool operator==(const LCPIterator& rhs) const {
	 return _terminated && rhs._terminated;
  }

  bool operator!=(const LCPIterator& rhs) const {
	 return !_terminated || !rhs._terminated;
  }

// This static method returns a sentinel LCPIterator used for testing if the streams are finished.
  static const LCPIterator& end() {
	 static const LCPIterator _end;
	 return _end;
  }


private:
  // no need of copy ctor nor assignment operator
  LCPIterator(): _terminated(true) { }
  LCPIterator(const LCPIterator& other) { }
  LCPIterator& operator=(const LCPIterator& other) { return *this; }
};

#endif
