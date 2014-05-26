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

#ifndef MULTI_FILE_MANAGER_H
#define MULTI_FILE_MANAGER_H

#include <vector>
#include <fstream>

#include "util.h"

template <typename TFile>
struct StdBuilder {
  TFile* operator()(const std::string& name)
  {
    return new TFile();
  }
};

template <typename TFile,
          typename TBuilder=StdBuilder<TFile> >
class MultiFileManager {

public:

  typedef TFile file_t;
  typedef TBuilder builder_t;
  typedef std::vector<file_t*> files_t;

  MultiFileManager(const std::string basename,
                   const std::string suffix= "")
      : basename_(basename), suffix_(suffix)
  {}

  ~MultiFileManager() {
    for (typename files_t::iterator it= files_.begin();
         it != files_.end();
         ++it) {
      delete *it;
    }
  }

  file_t& operator[](const size_t key) {
    while (key>=files_.size()) {
      const std::string skey= convert_to_string(files_.size());
      const std::string key_basename= basename_ + suffix_ + skey;
      files_.push_back(builder_(key_basename));
    }
    return *files_[key];
  }

  typename files_t::size_type size() const {
    return files_.size();
  }

private:

  builder_t builder_;
  const std::string basename_;
  const std::string suffix_;
  files_t files_;

};




template <>
struct StdBuilder<std::ofstream> {
  std::ofstream* operator()(const std::string& name)
  {
    return new std::ofstream(name.c_str(),
                             std::ios::out | std::ios::trunc | std::ios::binary);
  }
};

template <>
struct StdBuilder<std::ifstream> {
  std::ifstream* operator()(const std::string& name)
  {
    return new std::ifstream(name.c_str(),
                             std::ios::in | std::ios::binary);
  }
};

typedef MultiFileManager<std::ofstream, StdBuilder<std::ofstream> > OutputMultiFileManager;
typedef MultiFileManager<std::ifstream, StdBuilder<std::ifstream> > InputMultiFileManager;

#endif
