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

#include "file_abstractions.h"

template <typename TFile,
          typename TFileValue>
struct read_value_t {

  template <typename TValue>
  bool operator()(TFile& _file, TValue& value,
                  typename std::enable_if< std::is_same<TValue,TFileValue>::value>::type* dummy=0) const {
    return _file.read(value);
  }

  template <typename TValue>
  bool operator()(TFile& _file, TValue& value,
                  typename std::enable_if< !std::is_same<TValue,TFileValue>::value>::type* dummy=0) const {
    TFileValue val;
    const bool result= _file.read(val);
    if (result) {
      value= val;
      return true;
    }
    return false;
  }
};

template <typename TFileValue,
          typename TFile,
          typename TValue=TFileValue,
          typename TReader=read_value_t<TFile, TFileValue> >
class SingleFileIterator
{
public:
  typedef TFileValue filevalue_t;
  typedef TFile file_t;
  typedef TValue value_t;
  typedef TReader reader_t;

private:

  const reader_t _reader;
  const std::string _filename;
  file_t _file;
  value_t _current_value;

  BWTPosition _current_position;
  bool _terminated;


public:
  // Constructors
  SingleFileIterator(const std::string& filename)
    :_reader(), _filename(filename)
  {
	 open();
  }


  // Destructor
  ~SingleFileIterator() {
	 close();
  }


  SingleFileIterator& operator++() {
    _FAIL_IF_DBG(_terminated);
    ++_current_position;
    const bool valid = _reader(_file, _current_value);
    _terminated = !valid;
    return *this;
  }


  TValue operator*() const {
    _FAIL_IF_DBG(_terminated);
    return _current_value;
  }


  BWTPosition get_position() const {
    return _current_position;
  }


  void reset() {
    close();
    open();
  }


  bool terminated() const {
    return _terminated;
  }

private:
  // no need of copy ctor nor assignment operator
  SingleFileIterator();
  SingleFileIterator(const SingleFileIterator<TFileValue,TFile,TValue,TReader>&);
  SingleFileIterator<TFileValue,TValue>&
  operator=(const SingleFileIterator<TFileValue,TFile,TValue,TReader>&);

  void open() {
    DEBUG_LOG_VERBOSE("Initializing SingleFileIterator on file: " << _filename);
    const bool result= _file.open(_filename);
    if (!result) {
      DEBUG_LOG("Impossible to open file '" << _filename << "' for reading.");
      throw std::logic_error(std::string("Impossible to open file '")
                             + _filename + "' for reading.");
    }
    DEBUG_LOG_VERBOSE("File '" << _filename << "' successfully opened.");
    const bool valid = _reader(_file, _current_value);
    _current_position= 0;
    _terminated = !valid;
  }

  void close() {
    DEBUG_LOG_VERBOSE("Closing SingleFileIterator on file: " << _filename);
    _file.close();
    _current_position= 0;
    _terminated= true;
  }

};


#endif
