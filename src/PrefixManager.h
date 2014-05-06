#ifndef PREFIX_MANAGER_H
#define PREFIX_MANAGER_H

#include <string>
#include <fstream>

#include "types.h"
#include "util.h"

typedef SequenceNumber pref_mgr_elem_t;

class PrefixManager {

public:
  PrefixManager(const std::string& filename)
		:filename_(filename),
		 size_(0),
		 of_(filename, std::ios::out | std::ios::trunc | std::ios::binary)
  {}

  void push(const pref_mgr_elem_t& elem) {
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
  

private:
  const std::string filename_;
  SequenceNumber size_;
  std::ofstream of_;
  pref_mgr_elem_t top_;

};



#endif
