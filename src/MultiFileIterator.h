// -*- c++ -*-

/**
 *
 *                     LightStringGraph
 *
 *            Lightweight String Graph Construction.
 *
 * Copyright (C) 2013, 2014 Stefano Beretta, Yuri Pirola, Marco Previtali
 *
 * Distributed under the terms of the GNU General Public License (or the Lesser
 * GPL).
 *
 * This file is part of LightStringGraph.
 *
 * LighStringGraph is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * LightStringGraph is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with LightStringGraph.  If not, see <http://www.gnu.org/licenses/>.
 *
 **/

#ifndef MULTI_FILE_ITERATOR_H
#define MULTI_FILE_ITERATOR_H

#include <iostream>
#include <vector>
#include <string>
#include <iterator>

#include "types.h"
#include "util.h"
#include "SingleFileIterator.h"

template <typename TFileValue,
			 typename TValue=TFileValue,
			 typename TGetValue=get_value<TFileValue,TValue> >
class MultiFileIterator
  : public std::iterator< std::input_iterator_tag,
								  TValue >
{
private:
  typedef SingleFileIterator<TFileValue, TValue, TGetValue> file_iterator_t;
  typedef std::vector< file_iterator_t* > file_iterators_t;

  file_iterators_t  _iterators;
  typename file_iterators_t::iterator _current_iterator;

  BWTPosition _current_position;
  bool _terminated;

public:
  // Constructors
  MultiFileIterator(const std::vector< std::string >& filenames);

  // Destructor
  ~MultiFileIterator() {
    for (typename file_iterators_t::iterator it= _iterators.begin();
        it != _iterators.end();
        ++it) {
        delete *it;
    }
  }

  MultiFileIterator& operator++();

  TValue operator*() {
    _FAIL_IF(_terminated);
	 return ***_current_iterator;
  }

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
	 for (typename file_iterators_t::iterator it= _iterators.begin();
			it != _iterators.end();
			++it) {
		(**it).reset();
	 }
	 _current_iterator= _iterators.begin();
	 _terminated= (_current_iterator == _iterators.end());
	 while (!_terminated && (**_current_iterator) == file_iterator_t::end()) {
		++_current_iterator;
		_terminated |= (_current_iterator == _iterators.end());
	 }
	 _current_position= 0;
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

};

#include "util.h"

template <typename TFileValue, typename TValue, typename TGetValue >
MultiFileIterator<TFileValue,TValue,TGetValue>::\
MultiFileIterator(const std::vector< std::string >& filenames)
	 : _current_position(0)
{
  if (filenames.empty()) {
	 _FAIL_AND_LOG("Can't initialize a MultiFileIterator without filenames.");
  }

  for (std::vector<std::string>::const_iterator it= filenames.begin();
		 it != filenames.end();
		 ++it) {
	 _iterators.push_back(new file_iterator_t(*it));
  }

  _current_iterator= _iterators.begin();
  _terminated= (_current_iterator == _iterators.end());
  while (!_terminated && (**_current_iterator) == file_iterator_t::end()) {
	 ++_current_iterator;
	 _terminated |= (_current_iterator == _iterators.end());
  }
}

template <typename TFileValue, typename TValue, typename TGetValue >
MultiFileIterator<TFileValue,TValue,TGetValue>&
MultiFileIterator<TFileValue,TValue,TGetValue>::\
operator++() {
  if (!_terminated) {
	 ++_current_position;
	 ++(**_current_iterator);
	 while (!_terminated && (**_current_iterator) == file_iterator_t::end()) {
		++_current_iterator;
		_terminated |= (_current_iterator == _iterators.end());
	 }
  }
  return *this;
}


#endif
