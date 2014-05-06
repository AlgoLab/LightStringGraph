// -*- c++ -*-

#include "extend_symbol_pile.h"

ExtendSymbolPile::ExtendSymbolPile( vector< string >& filenames )
  : _filenamesIn( filenames )
{
  _mode = std::ios_base::out;
  _init_files( );
}

ExtendSymbolPile::~ExtendSymbolPile( )
{
  for( vector< std::fstream* >::iterator it = _files.begin();
       it != _files.end(); ++it)
    {
      (*it)->close();
      delete *it;
    }
  _files.clear();
}

void ExtendSymbolPile::_init_files( )
{
  for( vector< std::fstream* >::iterator it = _files.begin();
       it != _files.end(); ++it)
    {
      (*it)->close();
      delete *it;
    }

  _files.clear( );

  // Delete input files if new mode is input
  if( _mode == std::ios_base::out )
    {
      for(vector< string >::iterator it = _filenamesIn.begin();
          it != _filenamesIn.end(); ++it)
        if( remove( (*it).c_str( ) )  != 0 )
          {
            DEBUG_LOG( "Unable to remove file " << (*it).c_str() );
          }
          
    }

  for(vector< string >::iterator it = _filenamesIn.begin();
      it != _filenamesIn.end(); ++it )
    {
      _files.push_back( new std::fstream( (*it).c_str(), _mode ) );
    }
}

std::ios_base::openmode ExtendSymbolPile::switch_mode( )
{
  if( _mode == std::ios_base::out )
      _mode = std::ios_base::in;
  else
    _mode = std::ios_base::out;

  _init_files( );

  return _mode;
}

void ExtendSymbolPile::add_extend_symbol( vector< Nucleotide >& vnucl, const EdgeLength& l )
{
  assert( _mode == std::ios_base::in );
  (*_files[ l ]) << vnucl.size( );
  for( size_t i(0); i < vnucl.size(); ++i)
    (*_files[ l ]) << ntoc(vnucl[ i ]);
}

vector< Nucleotide > ExtendSymbolPile::get_next_symbol( EdgeLength& l )
{
  assert( _mode == std::ios_base::out );
  vector< Nucleotide > vnucl;
  size_t s;
  (*_files[ l ]) >> s;
  for( size_t i(0); i < s; ++i)
    {
      Nucleotide n;
      char c;
      (*_files[ l ]) >> c;
      n = cton(c);
      vnucl.push_back(n);
    }
  return vnucl;
}