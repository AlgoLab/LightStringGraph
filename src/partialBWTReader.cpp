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
#include <partialBWTReader.h>


partialBWTReader::partialBWTReader ( const string& inputFilename,
                                     BWTPosition start,
                                     const vector< NucleoCounter >& occurrencesBeforeStart )
    :_fileIn(NULL),
     _buffer(new char[ BUFFERSIZE ]),
     _start(start),
     _position(0),
     _occurrencesBeforeStart(occurrencesBeforeStart)
{
#ifdef HAS_ZLIB
  _fileIn= gzopen(inputFilename.c_str(), "r");
  if (_fileIn == Z_NULL) {
#else
  _fileIn= fopen(inputFilename.c_str(), "r");
  if (_fileIn == NULL) {
#endif
    DEBUG_LOG("Impossible to open file '" << inputFilename << "' for reading.");
    throw std::logic_error(std::string("Impossible to open file '")
                           + inputFilename + "' for reading.");
  }
  DEBUG_LOG("File '" << inputFilename << "' successfully opened.");
#ifdef HAS_ZLIB
  _bufferlen= gzread( _fileIn, _buffer, BUFFERSIZE );
#else
  _bufferlen= fread( _buffer, 1, BUFFERSIZE, _fileIn);
#endif
}

bool partialBWTReader::move_to ( const BWTPosition & p )
{
  if( p < _start + _position )
    {
      std::cerr << "Can't move to " << p << " because the current position is "
                << _start + _position << std::endl;
      std::cerr << "_start    = " << _start << std::endl;
      std::cerr << "_position = " << _position << std::endl;
      _MY_FAIL;
    }
  while ( ( _bufferlen != 0 ) &&
          ( _position < _bufferlen ) &&
          ( _start + _position < p ) )
    {
      char currentChar = _buffer[ _position ];
      ++_occurrencesBeforeStart[ NuclConv::cton( currentChar ) ];
      ++_position;
      if ( _position == _bufferlen )
        {
          _start += _bufferlen;
#ifdef HAS_ZLIB
          _bufferlen= gzread( _fileIn, _buffer, BUFFERSIZE );
#else
          _bufferlen= fread( _buffer, 1, BUFFERSIZE, _fileIn);
#endif
          _position = 0;
          if(_bufferlen == 0)
            {
              return false;
            }
        }
    }
  if( (_start + _position  < p) || ((_position == 0) && (_bufferlen == 0)) )
    {
      DEBUG_LOG_VERBOSE("ERROR: asked to reach position " << p << " but reached position " \
                        << _start + _position << " instead.");

      return false;
    }

  return true;
}
