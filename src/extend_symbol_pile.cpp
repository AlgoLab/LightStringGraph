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
 * along with Nome-Programma.  If not, see <http://www.gnu.org/licenses/>.
 *
 **/

#include "extend_symbol_pile.h"

ExtendSymbolPile::ExtendSymbolPile( vector< string >& filenames )
  : _filenamesIn( filenames )
{
  _mode = std::ios_base::out | std::ios_base::trunc;
  _init_files( );
}

ExtendSymbolPile::~ExtendSymbolPile( )
{
  for( vector< std::fstream* >::const_iterator it = _files.begin();
       it != _files.end(); ++it)
    {
      (*it)->close();
      delete *it;
    }
  for( vector< string >::const_iterator it = _filenamesIn.begin();
       it != _filenamesIn.end(); ++it )
    if( remove((*it).c_str()) != 0 )
        DEBUG_LOG( "Unable to remove file " << (*it).c_str() );
}

void ExtendSymbolPile::_init_files( )
{
  for( vector< std::fstream* >::const_iterator it = _files.begin();
       it != _files.end(); ++it)
    {
      (*it)->close();
      delete *it;
    }

  _files.clear( );

  for(vector< string >::const_iterator it = _filenamesIn.begin();
      it != _filenamesIn.end(); ++it )
    _files.push_back( new std::fstream( (*it).c_str(), _mode | std::ios_base::binary) );
}

std::ios_base::openmode ExtendSymbolPile::switch_mode( )
{
  if( _mode == std::ios_base::in )
    _mode = std::ios_base::out | std::ios_base::trunc;
  else
    _mode = std::ios_base::in;

  _init_files( );

  return _mode;
}

void ExtendSymbolPile::add_extend_symbol( vector< Nucleotide >& vnucl, const EdgeLength& l )
{
  assert( _mode == (std::ios_base::out | std::ios_base::trunc) );
  (*_files[ l ]) << (int)vnucl.size( );
  for( size_t i(0); i < vnucl.size(); ++i)
    (*_files[ l ]) << NuclConv::ntoc(vnucl[ i ]);
}

vector< Nucleotide > ExtendSymbolPile::get_next_symbol( EdgeLength& l )
{
  assert( _mode == std::ios_base::in );
  vector< Nucleotide > vnucl;
  int s;
  (*_files[ l ]) >> s;
  for( int i(0); i < s; ++i)
    {
      char c;
      (*_files[ l ]) >> c;
      vnucl.push_back(NuclConv::cton(c));
    }
  return vnucl;
}
