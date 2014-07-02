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

#ifndef ENDPOS_MANAGER_H
#define ENDPOS_MANAGER_H

#include <string>
#include <fstream>

#include "types.h"
#include "util.h"

#ifdef USE_BOOST_IOSTREAMS
#include <boost/iostreams/device/mapped_file.hpp>
#endif


class EndPosManager {

public:

  typedef SequenceNumber elem_t;
  typedef SequenceNumber position_t;
  typedef SequenceNumber offset_t;

  EndPosManager(const std::string& filename)
      :filename_(filename)
  {
#ifdef USE_BOOST_IOSTREAMS
    DEBUG_LOG("EndPosManager: using boost's mmap on file '" << filename_ << "'.");
	 mm_.open(filename_);
#else
    DEBUG_LOG("EndPosManager: using std::ifstream on file '" << filename_ << "'.");
	 if_.open(filename_, std::ios::in | std::ios::binary);
#endif
  }

  ~EndPosManager() {
  }

  void get_elems(const position_t& p, const offset_t& n_el,
                 std::vector<elem_t>& elems) {
    elems.clear();
    elems.resize(n_el);
#ifdef USE_BOOST_IOSTREAMS
    const elem_t* const data= reinterpret_cast<const elem_t*>(((char*)mm_.data())+(sizeof(SequenceNumber)+2*sizeof(uint8_t)));
    std::copy(data+p, data+p+n_el, elems.begin());
#else
    if_.seekg(p * sizeof(elem_t) + (sizeof(SequenceNumber)+2*sizeof(uint8_t)));
    if_.read(reinterpret_cast<char*>(&elems[0]), n_el*sizeof(elem_t));
    _FAIL_IF( !if_ );
#endif
  }

  elem_t get_elem(const position_t& p) {
	 elem_t result= 0;
#ifdef USE_BOOST_IOSTREAMS
    const elem_t* const data= reinterpret_cast<const elem_t*>(((char*)mm_.data())+(sizeof(SequenceNumber)+2*sizeof(uint8_t)));
	 result= *(data+p);
#else
    if_.seekg(p * sizeof(elem_t)+(sizeof(SequenceNumber)+2*sizeof(uint8_t)));
    if_.read(reinterpret_cast<char*>(&result), sizeof(elem_t));
    _FAIL_IF( !if_ );
#endif
	 return result;
  }

private:
  const std::string filename_;

#ifdef USE_BOOST_IOSTREAMS
  boost::iostreams::mapped_file_source mm_;
#else
  std::ifstream if_;
#endif

};



#endif
