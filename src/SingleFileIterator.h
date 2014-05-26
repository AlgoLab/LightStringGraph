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
 * along with Nome-Programma.  If not, see <http://www.gnu.org/licenses/>.
 *
 **/

#ifndef SINGLE_FILE_ITERATOR_H
#define SINGLE_FILE_ITERATOR_H

#include <fstream>
#include <string>
#include <iterator>


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
class SingleFileIterator
  : public std::iterator< std::input_iterator_tag,
								  TValue >
{
private:
  typedef std::istream_iterator<TFileValue> file_iterator_t;

  const file_iterator_t _eoi;
  TGetValue get_v;

  std::string _filename;
  std::ifstream _file;
  file_iterator_t*  _iterator;

  BWTPosition _current_position;
  bool _terminated;


public:
  // Constructors
  SingleFileIterator(const std::string& filename)
		:_filename(filename)
  {
	 open();
  }


  // Destructor
  ~SingleFileIterator() {
	 close();
  }


  SingleFileIterator& operator++() {
	 if (!_terminated) {
		++_current_position;
		++(*_iterator);
		_terminated= (*_iterator == _eoi);
	 }
	 return *this;
  }


  TValue operator*() {
    _FAIL_IF(_terminated);
	 return get_v(**_iterator);
  }


  BWTPosition get_position() const {
	 return _current_position;
  }


  void reset() {
	 close();
	 open();
  }


// This static method returns a sentinel SingleFileIterator used for testing if the stream is finished.
  static const SingleFileIterator<TFileValue,TValue,TGetValue>&
  end() {
	 static const SingleFileIterator<TFileValue,TValue,TGetValue> _end;
	 return _end;
  }


  bool
  operator==(const SingleFileIterator<TFileValue,TValue,TGetValue>& rhs) const {
	 return _terminated && rhs._terminated;
  }


  bool
  operator!=(const SingleFileIterator<TFileValue,TValue,TGetValue>& rhs) const {
	 return !_terminated || !rhs._terminated;
  }


private:
  // no need of copy ctor nor assignment operator
  SingleFileIterator(): _terminated(true) { }
  SingleFileIterator(const SingleFileIterator<TFileValue,TValue,TGetValue>& other) { }
  SingleFileIterator<TFileValue,TValue,TGetValue>&
  operator=(const SingleFileIterator<TFileValue,TValue,TGetValue>& other) { return *this; }

  void open() {
	 DEBUG_LOG_VERBOSE("Initializing SingleFileIterator on file: " << _filename);
	 _file.open(_filename);
	 _iterator= new file_iterator_t(_file);
	 _current_position= 0;
	 _terminated= (*_iterator == _eoi);
  }

  void close() {
	 DEBUG_LOG_VERBOSE("Closing SingleFileIterator on file: " << _filename);
	 if (_iterator != NULL) {
		delete _iterator;
		_iterator= NULL;
	 }
	 if (_file.is_open())
		_file.close();
	 _current_position= 0;
	 _terminated= true;
  }

};


#endif
