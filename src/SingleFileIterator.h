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

#ifndef SINGLE_FILE_ITERATOR_H
#define SINGLE_FILE_ITERATOR_H

#include <string>
#include <cstdlib>

#include <zlib.h>

template <typename TFile,
          typename TFileValue,
          typename TValue>
struct read_value_t;

template <typename TFileValue,
          typename TValue>
struct read_value_t<gzFile, TFileValue, TValue> {
  bool operator()(gzFile _file, TValue& value) const {
    TFileValue val;
    const int byteread= gzread( _file, &val, sizeof(val) );
    if (byteread == sizeof(val)) {
      value= val;
      return true;
    }
    return false;
  }
};

template <typename TFileValue,
          typename TValue=TFileValue,
          typename TReader=read_value_t<gzFile, TFileValue, TValue> >
class SingleFileIterator
{
public:
  typedef TFileValue filevalue_t;
  typedef TValue value_t;
  typedef TReader reader_t;

private:

  const reader_t _reader;
  const std::string _filename;
  gzFile _file;
  value_t _current_value;

  BWTPosition _current_position;
  bool _terminated;


public:
  // Constructors
  SingleFileIterator(const std::string& filename)
    :_reader(), _filename(filename), _file(NULL)
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
      const bool valid = _reader(_file, _current_value);
      _terminated = !valid;
    }
    return *this;
  }


  TValue operator*() {
    _FAIL_IF(_terminated);
    return _current_value;
  }


  BWTPosition get_position() const {
    return _current_position;
  }


  void reset() {
    close();
    open();
  }


// This static method returns a sentinel SingleFileIterator used for testing if the stream is finished.
  static const SingleFileIterator<TFileValue,TValue,TReader>&
  end() {
    static const SingleFileIterator<TFileValue,TValue,TReader> _end;
    return _end;
  }


  bool
  operator==(const SingleFileIterator<TFileValue,TValue,TReader>& rhs) const {
	 return _terminated && rhs._terminated;
  }


  bool
  operator!=(const SingleFileIterator<TFileValue,TValue,TReader>& rhs) const {
	 return !_terminated || !rhs._terminated;
  }


private:
  // no need of copy ctor nor assignment operator
  SingleFileIterator(): _reader(),_terminated(true) { }
  SingleFileIterator(const SingleFileIterator<TFileValue,TValue,TReader>&);
  SingleFileIterator<TFileValue,TValue>&
  operator=(const SingleFileIterator<TFileValue,TValue,TReader>&);

  void open() {
    DEBUG_LOG_VERBOSE("Initializing SingleFileIterator on file: " << _filename);
    _file= gzopen(_filename.c_str(), "rb");
    if (_file == Z_NULL) {
      DEBUG_LOG("Impossible to open file '" << _filename << "' for reading.");
      throw std::logic_error(std::string("Impossible to open file '")
                             + _filename + "' for reading.");
    }
    DEBUG_LOG("File '" << _filename << "' successfully opened.");
    const bool valid = _reader(_file, _current_value);
    _current_position= 0;
    _terminated = !valid;
  }

  void close() {
    DEBUG_LOG_VERBOSE("Closing SingleFileIterator on file: " << _filename);
    if (_file != Z_NULL) {
      gzclose(_file);
      _file = Z_NULL;
    }
    _current_position= 0;
    _terminated= true;
  }

};


#endif
