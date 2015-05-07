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

#include <vector>
#include <string>

#include "types.h"
#include "util.h"
#include "SingleFileIterator.h"

template <typename TFileValue,
          typename TFile,
          typename TValue=TFileValue,
          typename TReader=read_value_t<TFile, TFileValue> >
class MultiFileIterator
{
private:
  typedef SingleFileIterator<TFileValue, TFile, TValue, TReader> file_iterator_t;
  typedef std::vector< file_iterator_t* > file_iterators_t;

  file_iterators_t  _iterators;
  typename file_iterators_t::const_iterator _current_iterator;

  BWTPosition _current_position;
  bool _terminated;

public:
  // Constructors
  MultiFileIterator(const std::vector< std::string >& filenames);

  // Destructor
  ~MultiFileIterator() {
    for (typename file_iterators_t::const_iterator it= _iterators.begin();
        it != _iterators.end();
        ++it) {
        delete *it;
    }
  }

  MultiFileIterator& operator++() {
    _FAIL_IF_DBG(_terminated);
    ++_current_position;
    ++(**_current_iterator);
    while (!_terminated && (*_current_iterator)->terminated()) {
      ++_current_iterator;
      _terminated |= (_current_iterator == _iterators.end());
    }
    return *this;
  }

  TValue operator*() const {
    _FAIL_IF_DBG(_terminated);
    return ***_current_iterator;
  }

  BWTPosition get_position() const {
	 return _current_position;
  }

  void reset() {
	 for (typename file_iterators_t::const_iterator it= _iterators.begin();
			it != _iterators.end();
			++it) {
		(**it).reset();
	 }
	 _current_iterator= _iterators.begin();
	 _terminated= (_current_iterator == _iterators.end());
	 while (!_terminated && (*_current_iterator)->terminated()) {
		++_current_iterator;
		_terminated |= (_current_iterator == _iterators.end());
	 }
	 _current_position= 0;
  }

  bool terminated() const {
    return _terminated;
  }


private:
  // no need of copy ctor nor assignment operator
  MultiFileIterator();
  MultiFileIterator(const MultiFileIterator<TFileValue,TFile,TValue,TReader>& );
  MultiFileIterator& operator=(const MultiFileIterator<TFileValue,TFile,TValue,TReader>& );

};

#include "util.h"

template <typename TFileValue, typename TFile, typename TValue, typename TReader>
MultiFileIterator<TFileValue,TFile,TValue,TReader>::     \
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
  while (!_terminated && (*_current_iterator)->terminated()) {
	 ++_current_iterator;
	 _terminated |= (_current_iterator == _iterators.end());
  }
}


#endif
