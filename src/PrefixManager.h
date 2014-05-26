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

#ifndef PREFIX_MANAGER_H
#define PREFIX_MANAGER_H

#include <string>
#include <fstream>

#include "types.h"
#include "util.h"

#ifdef USE_BOOST_IOSTREAMS
#include <boost/iostreams/device/mapped_file.hpp>
#elif defined USE_BGZF
#include "bgzf.h"
#endif


class PrefixManager {

public:

  typedef SequenceNumber elem_t;

#ifdef USE_BOOST_IOSTREAMS
  typedef SequenceNumber position_t;
#elif defined USE_BGZF
  typedef int64_t position_t;
#else
  typedef SequenceNumber position_t;
#endif

  typedef SequenceNumber offset_t;

  PrefixManager(const std::string& filename)
      :filename_(filename),
       size_(0),
       writing(false),
       reading(false)
  {
#ifdef USE_BOOST_IOSTREAMS
    DEBUG_LOG("PrefixManager: using boost's mmap.");
#elif defined USE_BGZF
    DEBUG_LOG("PrefixManager: using BGZF.");
#else
    DEBUG_LOG("PrefixManager: using std::ifstream.");
#endif
  }

  ~PrefixManager() {
#ifdef USE_BGZF
    bgzf_close(f_);
#endif
  }

  void push(const elem_t& elem) {
    _FAIL_IF(reading);
    if (!writing) {
#ifdef USE_BGZF
      f_= bgzf_open(filename_.c_str(), "w5");
#else
      of_.open(filename_.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
#endif
      writing= true;
    }

#ifdef USE_BGZF
    bgzf_write(f_, reinterpret_cast<const void*>(&elem), sizeof(elem_t));
#else
    of_.write(reinterpret_cast<const char*>(&elem), sizeof(elem_t));
#endif
    top_= elem;
    ++size_;
  }

  void flush() {
    if (writing) {
#ifdef USE_BGZF
      bgzf_flush(f_);
#else
      of_.flush();
#endif
    }
  }

  elem_t top() const {
    _FAIL_IF(size_ == 0);
    return top_;
  }

  offset_t size() const {
    return size_;
  }

  position_t position() const {
#ifdef USE_BGZF
    return bgzf_tell(f_);
#else
    return size_;
#endif
  }

  void get_elems(const position_t& p, const offset_t& n_el,
                 std::vector<elem_t>& elems) {
    if (!reading) {
      flush();
#ifdef USE_BOOST_IOSTREAMS
      if (writing) of_.close();
      mm_.open(filename_);
#elif defined USE_BGZF
      if (writing) {
        bgzf_flush(f_);
        bgzf_close(f_);
      }
      f_= bgzf_open(filename_.c_str(), "r");
#else
      if (writing) of_.close();
      if_.open(filename_, std::ios::in | std::ios::binary);
#endif
      writing= false;
      reading= true;
    }
    elems.clear();
    elems.resize(n_el);
#ifdef USE_BOOST_IOSTREAMS
    const elem_t* const data= reinterpret_cast<const elem_t*>(mm_.data());
    std::copy(data+p, data+p+n_el, elems.begin());
#elif defined USE_BGZF
    const int64_t seek_result= bgzf_seek(f_, p, SEEK_SET);
    _FAIL_IF(seek_result != 0);
    const ssize_t read_result= bgzf_read(f_,
                                         reinterpret_cast<void*>(&elems[0]),
                                         n_el*sizeof(elem_t));
    _FAIL_IF(read_result != (n_el*sizeof(elem_t)));
#else
    if_.seekg(p * sizeof(elem_t));
    if_.read(reinterpret_cast<char*>(&elems[0]), n_el*sizeof(elem_t));
    _FAIL_IF( !if_ );
#endif
  }

private:
  const std::string filename_;
  offset_t size_;

#ifdef USE_BOOST_IOSTREAMS
  std::ofstream of_;
  boost::iostreams::mapped_file_source mm_;
#elif defined USE_BGZF
  BGZF* f_;
#else
  std::ofstream of_;
  std::ifstream if_;
#endif

  elem_t top_;

  bool writing;
  bool reading;

};



#endif
