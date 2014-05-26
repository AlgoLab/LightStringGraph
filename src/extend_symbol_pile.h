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
