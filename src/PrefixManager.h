#ifndef PREFIX_MANAGER_H
#define PREFIX_MANAGER_H

#include <string>
#include <fstream>

#include "types.h"
#include "util.h"

#ifdef USE_BOOST_IOSTREAMS
#include <boost/iostreams/device/mapped_file.hpp>
#endif

typedef SequenceNumber pref_mgr_elem_t;

class PrefixManager {

public:
  PrefixManager(const std::string& filename)
		:filename_(filename),
		 size_(0),
		 of_(filename, std::ios::out | std::ios::trunc | std::ios::binary),
		 reading(false)
  {}

  void push(const pref_mgr_elem_t& elem) {
	 _FAIL_IF(reading);
	 of_.write(reinterpret_cast<const char*>(&elem), sizeof(pref_mgr_elem_t));
	 top_= elem;
	 ++size_;
  }

  pref_mgr_elem_t top() const {
	 _FAIL_IF(size_ == 0);
	 return top_;
  }

  SequenceNumber size() const {
	 return size_;
  }

  void get_elems(const SequenceNumber& b, const SequenceNumber& e,
					  std::vector<pref_mgr_elem_t>& elems) {
	 if (!reading) {
		of_.close();
#ifdef USE_BOOST_IOSTREAMS
		mm_.open(filename_);
#else
		if_.open(filename_, std::ios::in | std::ios::binary);
#endif
		reading= true;
	 }
	 _FAIL_IF( (b >= e) || (e > size_));
	 elems.clear();
	 elems.resize(e-b);
#ifdef USE_BOOST_IOSTREAMS
	 const pref_mgr_elem_t* data= reinterpret_cast<const pref_mgr_elem_t*>(mm_.data());
	 std::copy(data+b, data+e, elems.begin());
#else
	 if_.seekg(b * sizeof(pref_mgr_elem_t));
	 if_.read(reinterpret_cast<char*>(&elems[0]), (e-b)*sizeof(pref_mgr_elem_t));
	 _FAIL_IF( !if_ );
#endif
  }

private:
  const std::string filename_;
  SequenceNumber size_;
  std::ofstream of_;

#ifdef USE_BOOST_IOSTREAMS
  boost::iostreams::mapped_file_source mm_;
#else
  std::ifstream if_;
#endif

  pref_mgr_elem_t top_;

  bool reading;

};



#endif
