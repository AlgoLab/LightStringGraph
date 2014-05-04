// -*- c++ -*-
#ifndef GSA_ITERATOR_H
#define GSA_ITERATOR_H

#include <fstream>
#include <vector>
#include <string>
#include <iterator>

#include "types.h"

std::istream& operator>> (std::istream& is, GSAEntry& li);

class GSAIterator
  : public std::iterator< std::input_iterator_tag,
								  GSAEntry >
{
private:
  typedef std::istream_iterator<GSAEntry> gsa_iterator_t;

  const std::istream_iterator<GSAEntry> _eoi;
  std::ifstream _file;
  gsa_iterator_t  _iterator;

  BWTPosition _current_position;
  bool _terminated;


public:
  // Constructors
  GSAIterator(const std::string& filename);

  // Destructor
  ~GSAIterator();

  GSAIterator& operator++();
  GSAEntry operator*();

  BWTPosition get_position() const {
	 return _current_position;
  }

  bool operator==(const GSAIterator& rhs) const {
	 return _terminated && rhs._terminated;
  }

  bool operator!=(const GSAIterator& rhs) const {
	 return !_terminated || !rhs._terminated;
  }

// This static method returns a sentinel GSAIterator used for testing if the stream is finished.
  static const GSAIterator& end() {
	 static const GSAIterator _end;
	 return _end;
  }


private:
  // no need of copy ctor nor assignment operator
  GSAIterator(): _terminated(true) { }
  GSAIterator(const GSAIterator& other) { }
  GSAIterator& operator=(const GSAIterator& other) { return *this; }
};

#endif
