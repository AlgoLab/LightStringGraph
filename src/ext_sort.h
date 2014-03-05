#ifndef EXT_SORT_H
#define EXT_SORT_H

#include "types.h"
#include "util.h"

#define BITS_OF_BWT_POSITION sizeof(BWTPosition) * 8
#define MERGED_FILES std::string("merged_files")

#include <string>
#include <fstream>
#include <cstdlib>
#include <iostream>

using std::string;

// External memory sorting of extension pairs

std::ifstream* ext_sort( size_t elem_num, string filname_in );

#endif
