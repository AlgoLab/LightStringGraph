// -*- c++ -*-

#ifndef EXTEND_SYMBOL_PILE_H
#define EXTEND_SYMBOL_PILE_H

#include <fstream>
#include <vector>
#include <string>
#include <cassert>
#include <cstddef>

#include "types.h"
#include "util.h"

using std::vector;
using std::string;

class ExtendSymbolPile
{
private:
  vector< string > _filenamesIn;
  vector< std::fstream* > _files;
  std::ios_base::openmode _mode;

public:
  // Constructor
  ExtendSymbolPile( vector< string >& filenames );

  // Destructor
  ~ExtendSymbolPile( );

  // Add extend symbol
  void add_extend_symbol( vector< Nucleotide >&, const EdgeLength& );

  // Get next symbol
  vector< Nucleotide > get_next_symbol( EdgeLength& );

  // Switch from write to read
  std::ios_base::openmode switch_mode( );

private:
  // Init file as output files
  void _init_files( );
};

#endif
