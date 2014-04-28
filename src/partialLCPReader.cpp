// -*- c++ -*-
#include "partialLCPReader.h"

partialLCPReader::partialLCPReader(std::string& inputFilename)
{
  _fileIN.open(inputFilename.c_str());
  if(_fileIN.fail())
    {
      std::cerr << "ERROR: Can't open file " << inputFilename << std::endl
                << "Aborting." << std::endl;
      std::exit(-1);
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
      std::exit(-1);
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
