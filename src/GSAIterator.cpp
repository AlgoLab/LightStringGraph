// -*- c++ -*-

#include "util.h"
#include "GSAIterator.h"

std::istream& operator>> (std::istream& is, GSAEntry& li) {
  is.read(reinterpret_cast<char*>(&li), sizeof(GSAEntry));
  return is;
}

GSAIterator::GSAIterator(const std::string& filename)
	 : _file(filename.c_str()),
		_iterator(_file),
		_current_position(0)
{
  DEBUG_LOG_VERBOSE("Initializing GSAIterator on file: " << filename);
  _terminated= (_iterator == _eoi);
}

GSAIterator::~GSAIterator()
{
}

GSAIterator& GSAIterator::operator++() {
  if (!_terminated) {
	 ++_current_position;
	 ++_iterator;
	 _terminated= (_iterator == _eoi);
  }
  return *this;
}

GSAEntry GSAIterator::operator*() {
  _FAIL_IF(_terminated);
  return *_iterator;
}
