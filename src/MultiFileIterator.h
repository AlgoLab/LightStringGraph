// -*- c++ -*-
#ifndef MULTI_FILE_ITERATOR_H
#define MULTI_FILE_ITERATOR_H

#include <iostream>
#include <vector>
#include <string>
#include <iterator>

#include "types.h"
#include "util.h"

template <typename TFileValue,
			 typename TValue=TFileValue>
struct get_value {
  TValue operator()(const TFileValue& v) {
	 return TValue(v);
  }
};


template <typename TFileValue,
			 typename TValue=TFileValue,
			 typename TGetValue=get_value<TFileValue,TValue> >
class MultiFileIterator
  : public std::iterator< std::input_iterator_tag,
								  TValue >
{
private:
  typedef std::istream_iterator<TFileValue> file_iterator_t;
  typedef std::vector< file_iterator_t > file_iterators_t;
  typedef std::vector< std::string > filenames_t;
  typedef std::vector< std::ifstream* > files_t;

  const file_iterator_t _eoi;
  TGetValue get_v;
  const filenames_t _filenames;
  files_t  _files;
  file_iterators_t  _iterators;
  typename file_iterators_t::iterator _current_iterator;

  BWTPosition _current_position;
  bool _terminated;


public:
  // Constructors
  MultiFileIterator(const std::vector< std::string >& filenames);

  // Destructor
  ~MultiFileIterator();

  MultiFileIterator& operator++();
  TValue operator*();

  BWTPosition get_position() const {
	 return _current_position;
  }

  bool operator==(const MultiFileIterator<TFileValue,TValue,TGetValue>& rhs) const {
	 return _terminated && rhs._terminated;
  }

  bool operator!=(const MultiFileIterator<TFileValue,TValue,TGetValue>& rhs) const {
	 return !_terminated || !rhs._terminated;
  }

  void reset() {
	 close_all();
	 open_all();
  }

// This static method returns a sentinel MultiFileIterator used for testing if the streams are finished.
  static const MultiFileIterator<TFileValue,TValue,TGetValue>& end() {
	 static const MultiFileIterator<TFileValue,TValue,TGetValue> _end;
	 return _end;
  }


private:
  // no need of copy ctor nor assignment operator
  MultiFileIterator(): _terminated(true) { }
  MultiFileIterator(const MultiFileIterator<TFileValue,TValue,TGetValue>& other) { }
  MultiFileIterator& operator=(const MultiFileIterator<TFileValue,TValue,TGetValue>& other) { return *this; }

  void close_all() {
	 _iterators.clear();
	 _current_iterator= _iterators.end();
	 for (typename files_t::iterator it= _files.begin();
			it != _files.end();
			++it) {
		if (*it != NULL) {
		  delete *it;
		}
	 }
	 _files.clear();
	 _terminated= true;
  }

  void open_all() {
	 DEBUG_LOG_VERBOSE("Initializing MultiFileIterator on files:");
	 for(typename filenames_t::const_iterator it = _filenames.begin();
		  it != _filenames.end();
		  ++it) {
		DEBUG_LOG_VERBOSE("   " << *it);
		_files.push_back(new std::ifstream(it->c_str()));
		_iterators.push_back(file_iterator_t(*_files.back()));
	 }
	 _current_iterator= _iterators.begin();
	 _terminated= (_current_iterator == _iterators.end());
	 while (!_terminated && (*_current_iterator) == _eoi) {
		++_current_iterator;
		_terminated |= (_current_iterator == _iterators.end());
	 }
	 _current_position= 0;
  }

};

#include "util.h"

template <typename TFileValue, typename TValue, typename TGetValue >
MultiFileIterator<TFileValue,TValue,TGetValue>::\
MultiFileIterator(const std::vector< std::string >& filenames)
	 : _filenames(filenames),
		_current_position(0)
{
  if (filenames.empty()) {
	 _FAIL_AND_LOG("Can't initialize a MultiFileIterator without filenames.");
  }

  open_all();
}

template <typename TFileValue, typename TValue, typename TGetValue >
MultiFileIterator<TFileValue,TValue,TGetValue>::\
~MultiFileIterator()
{
  close_all();
}

template <typename TFileValue, typename TValue, typename TGetValue >
MultiFileIterator<TFileValue,TValue,TGetValue>&
MultiFileIterator<TFileValue,TValue,TGetValue>::\
operator++() {
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


template <typename TFileValue, typename TValue, typename TGetValue >
TValue
MultiFileIterator<TFileValue,TValue,TGetValue>::\
operator*() {
  _FAIL_IF(_terminated);
  return get_v(**_current_iterator);
}

#endif
