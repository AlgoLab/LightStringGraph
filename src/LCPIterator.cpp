// -*- c++ -*-

#include "LCPIterator.h"

#include "util.h"

std::istream& operator>> (std::istream& is, LCPFileValue& li) {
  is.read(reinterpret_cast<char*>(&li.v), sizeof(LCPFileValue));
  return is;
}

LCPIterator::LCPIterator(const std::vector< std::string >& filenames)
	 : _files(filenames.size(), NULL),
		_current_position(0)
{
  if (filenames.empty()) {
	 _FAIL_AND_LOG("Can't initialize an LCPIterator without filenames.");
  }

  DEBUG_LOG_VERBOSE("Initializing LCPIterator on files:");
  for(std::vector<std::string>::const_iterator it = filenames.begin();
      it != filenames.end();
		++it) {
	 DEBUG_LOG_VERBOSE("   " << *it);
	 _files.push_back(new std::ifstream(it->c_str()));
	 _iterators.push_back(std::istream_iterator<LCPFileValue>(*_files.back()));
  }
  _current_iterator= _iterators.begin();
  _terminated= (_current_iterator == _iterators.end());
  while (!_terminated && (*_current_iterator) == _eoi) {
	 ++_current_iterator;
	 _terminated |= (_current_iterator == _iterators.end());
  }
}

LCPIterator::~LCPIterator()
{
  for (std::vector<std::ifstream*>::iterator it= _files.begin();
		 it != _files.end();
		 ++it) {
	 if (*it != NULL)
		delete *it;
  }
}

LCPIterator& LCPIterator::operator++() {
  if (!_terminated) {
	 ++_current_position;
	 ++(*_current_iterator);
	 while (!_terminated && (*_current_iterator) == _eoi) {
		++_current_iterator;
		_terminated |= (_current_iterator == _iterators.end());
	 }
  }
  return *this;
}

LCPValue LCPIterator::operator*() {
  _FAIL_IF(_terminated);
  return (**_current_iterator).v;
}
