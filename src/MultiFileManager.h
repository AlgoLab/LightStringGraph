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
