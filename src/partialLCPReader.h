// -*- c++ -*-

#ifndef PARTIAL_LCPREADER_H
#define PARTIAL_LCPREADER_H

#include <vector>
#include <string>

#include "util.h"

class partialLCPReader
{
private:
  std::ifstream   _fileIN;
  LCPValue*       _buffer;
  BWTPosition     _start;
  BWTPosition     _position;
  BWTPosition     _bufferlen;

public:
  // Constructor
  partialLCPReader(std::string& inputFilename);
  partialLCPReader(std::string& inputFilename, BWTPosition start);

  // Destructor
  ~partialLCPReader();

  // Get absolute position
  BWTPosition get_position() const;

  // Get current value
  LCPValue get() const;

  // Move one step
  bool next();
};

#endif
