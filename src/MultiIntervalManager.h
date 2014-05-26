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

#ifndef MULTI_INTERVAL_MANAGER_H
#define MULTI_INTERVAL_MANAGER_H

#include <vector>

#include "util.h"
#include "interval_manager.h"

template <typename TInterval>
class MultiIntervalManager {

public:

  typedef TInterval interval_t;
  typedef IntervalManager<interval_t> interval_mgr_t;
  typedef std::vector<interval_mgr_t*> interval_mgrs_t;

  MultiIntervalManager(const std::vector<std::string> basenames,
                       const std::string suffix= "-K_")
      :basenames_(basenames), suffix_(suffix)
  {}

  ~MultiIntervalManager() {
    for (typename interval_mgrs_t::iterator it= int_mgrs_.begin();
         it != int_mgrs_.end();
         ++it) {
      delete *it;
    }
  }

  interval_mgr_t& operator[](const size_t key) {
    while (key>=int_mgrs_.size()) {
      const std::string skey= convert_to_string(int_mgrs_.size());
      std::vector<std::string> new_basenames;
      for (std::vector<std::string>::const_iterator it= basenames_.begin();
           it != basenames_.end();
           ++it) {
        new_basenames.push_back(*it + suffix_ + skey);
      }
      int_mgrs_.push_back(new interval_mgr_t(new_basenames));
    }
    return *int_mgrs_[key];
  }

  typename interval_mgrs_t::size_type size() const {
    return int_mgrs_.size();
  }

  void swap_all_files() {
    for (typename interval_mgrs_t::iterator it= int_mgrs_.begin();
         it != int_mgrs_.end();
         ++it) {
      (*it)->swap_files();
    }
  }


private:

  const std::vector<std::string> basenames_;
  const std::string suffix_;
  interval_mgrs_t int_mgrs_;

};



#endif
