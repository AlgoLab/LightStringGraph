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

#include "partialLCPReader.h"

partialLCPReader::partialLCPReader(std::string& inputFilename)
{
  _fileIN.open(inputFilename.c_str());
  if(_fileIN.fail())
    {
      std::cerr << "ERROR: Can't open file " << inputFilename << std::endl
                << "Aborting." << std::endl;
      _MY_FAIL;
    }

  _start    = 0;
  _position = 0;

  _buffer = new LCPValue[ BUFFERSIZE ];
  _fileIN.read((char*)_buffer, BUFFERSIZE * sizeof(LCPValue));
  _bufferlen = _fileIN.gcount() / sizeof(LCPValue);
}

partialLCPReader::partialLCPReader(std::string& inputFilename, BWTPosition start)
{
  _fileIN.open(inputFilename.c_str());
  if(_fileIN.fail())
    {
      std::cerr << "ERROR: Can't open file " << inputFilename << std::endl
                << "Aborting." << std::endl;
      _MY_FAIL;
    }

  _start = start;
  _position = 0;

  _buffer = new LCPValue[ BUFFERSIZE ];
  _fileIN.read((char *)_buffer, BUFFERSIZE * sizeof(LCPValue));
  _bufferlen = _fileIN.gcount() / sizeof(LCPValue);
}

partialLCPReader::~partialLCPReader()
{
  delete[] _buffer;
  _fileIN.close();
}

BWTPosition partialLCPReader::get_position() const
{
  return _start + _position;
}

LCPValue partialLCPReader::get() const
{
  return _buffer[ _position ];
}

bool partialLCPReader::next()
{
  ++_position;
  if(_position == _bufferlen)
    {
      _start += _bufferlen;
      _position = 0;
      _fileIN.read((char *)_buffer, BUFFERSIZE * sizeof(LCPValue));
      _bufferlen = _fileIN.gcount() / sizeof(LCPValue);
      if(_bufferlen == 0)
        return false;
    }
  return true;
}
